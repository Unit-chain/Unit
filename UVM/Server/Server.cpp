//
// Created by Alexander Syrgulev on 18.06.2022.
//

#include "Server.h"

namespace my_program_state {
    std::size_t request_count() {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t now() {
        return std::time(0);
    }
}

bool http_connection::push_transaction(std::string &transaction) {
    nlohmann::json transaction_json;
    try {
        transaction_json = nlohmann::json::parse(transaction);
    } catch (std::exception &e) {
        return false;
    }
    if  (!transaction_json.contains("extradata") || !transaction_json["extradata"].contains("name") || !transaction_json["extradata"].contains("value") || !transaction_json["extradata"].contains("bytecode")) return false;
    if  (transaction_json["extradata"]["name"].empty() || transaction_json["extradata"]["value"].empty() || transaction_json["extradata"]["bytecode"].empty()) return false;
    std::string extradata_name = to_string(transaction_json["extradata"]["name"]);
    std::string extradata_value = to_string(transaction_json["extradata"]["value"]);
    std::string extradata_bytecode = to_string(transaction_json["extradata"]["bytecode"]);
    extradata_name.erase(
            std::remove(extradata_name.begin(), extradata_name.end(), '\"'),
            extradata_name.end());
    extradata_value.erase(
            std::remove(extradata_value.begin(), extradata_value.end(), '\"'),
            extradata_value.end());
    extradata_bytecode.erase(
            std::remove(extradata_bytecode.begin(), extradata_bytecode.end(), '\"'),
            extradata_bytecode.end());
    std::map<std::string, std::string> map = {{"name", extradata_name}, {"value", to_string(transaction_json["extradata"]["value"])}, {"bytecode", extradata_bytecode}};
    std::string from = to_string(transaction_json["from"]);
    std::string to = to_string(transaction_json["to"]);
    from.erase(
            std::remove(from.begin(), from.end(), '\"'),
            from.end());
    to.erase(
            std::remove(to.begin(), to.end(), '\"'),
            to.end());
    Transaction tx = Transaction(from, to, transaction_json["type"],  map, "0", transaction_json["amount"]);
    Blockchain_db blockchainDb = Blockchain_db();
    if (!blockchainDb.validate_sender_balance(tx)) return false;
    this->tx_deque->push_back(tx);
    return true;
}

void http_connection::start(std::deque<Transaction> *deque) {
    this->tx_deque = deque;
    read_request();
    check_deadline();
}

// Asynchronously receive a complete request message.
void http_connection::read_request() {
    auto self = shared_from_this();
    http::async_read(
            socket_,
            buffer_,
            request_,
            [self](beast::error_code ec,
                   std::size_t bytes_transferred){
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->process_request();
            });
}

bool http_connection::valid_token_name(const std::string &token_name) {
    return true;
}

// Determine what needs to be done with the request message.
void http_connection::process_request() {
    response_.version(request_.version());
    response_.keep_alive(false);
    std::string string_body_test;
    nlohmann::json json;
    bool fl = false;
    switch (request_.method()) {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            this->create_response();
            break;
        case http::verb::post:
            /*
             * {
             * instruction:
             * data: {
             *         type:
             *         ...
             *         }
             * }
             *
             *
             * type 0 = unit transfer
             * type 1 = token transfer
             * type 2 = create token
             * -----------------------
             * if type == 0, we need to check if transaction's field "to" not equals to field "from" and "amount" not 0 or null (DONE)
             * if type == 1, we need to check if extradata is valid for transferring tokens: name equals to token name and value equals to amount of transffering tokens (DONE)
             * if type == 2, we need to check if extradata's bytecode can be parsed to json(try-catch) and contains fields: supply and name (DONE)
             *
             * */
            response_.set(http::field::content_type, "text/plain");
            try {
                json = nlohmann::json::parse(request_.body());
                std::string instruction = json["instruction"];
                nlohmann::json data = json["data"];
                std::string fixer = data["from"];

                int type = data["type"];
                // TYPE 0
                if (type == 0) {
                    // Checking first condition:
                    // nullptr when no amount
                    fl = (data["amount"] != nullptr &&
                          data["to"] != nullptr &&
                          data["from"] != nullptr &&
                          ((std::string)data["to"] != (std::string)data["from"]) &&
                          (int)data["amount"] != 0);
                std::string tx = data.dump(0);
                bool transaction_push = this->push_transaction(tx);
                // TYPE 1
                } else if (type == 1) {
                    std::cout << "type = 1 (token transfer)\n";
                    // Checking second condition:
                    fl = (data["extradata"]["value"] != nullptr &&
                          data["amount"] != nullptr &&
                          (int)data["extradata"]["value"] == (int)data["amount"] &&
                          valid_token_name(data["extradata"]["name"])); // valid_token_name - is in progress
                // TYPE 2
                } else if (type == 2) {
                    std::cout << "type = 2 (create token)\n";
                    std::string bytecode = data["extradata"]["bytecode"];
                    std::string bytecode_string = hex_to_ascii(bytecode);
                    nlohmann::json tmp = nlohmann::json::parse(bytecode_string);
                    // Checking third condition:
                    std::string name = tmp["name"];
                    int supply = tmp["supply"];
                    std::cout << "name = " << name << "\tsupply = " << supply << "\n";
                    fl = true;
                } else {
                    std::cout << "type = Unknown\n";
                }
            } catch (std::exception &e) {
                response_.result(http::status::not_found);
                nlohmann::json out = {{"Error", "true"},
                                      {"Result", e.what()}};
                std::cout << "Error out_json: " << to_string(out) << "\n";
                beast::ostream(response_.body()) << to_string(out);
                break;
            }
//            std::cout << "\nFlag is " << fl << "\n";
            response_.result(http::status::ok);
//            std::cout << json["data"] << std::endl;
            break;
        default:
            // We return responses indicating an error if
            // we do not recognize the request method.
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body())
                    << "Invalid request-method '"
                    << std::string(request_.method_string())
                    << "'";
            break;
    }

    write_response();
}

void http_connection::create_response() {
    if(request_.target() == "/count")
    {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
                << "<html>\n"
                <<  "<head><title>Request count</title></head>\n"
                <<  "<body>\n"
                <<  "<h1>Request count</h1>\n"
                <<  "<p>There have been "
                <<  my_program_state::request_count()
                <<  " requests so far.</p>\n"
                <<  "</body>\n"
                <<  "</html>\n";
    } else if(request_.target() == "/time") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
                <<  "<html>\n"
                <<  "<head><title>Current time</title></head>\n"
                <<  "<body>\n"
                <<  "<h1>Current time</h1>\n"
                <<  "<p>The current time is "
                <<  my_program_state::now()
                <<  " seconds since the epoch.</p>\n"
                <<  "</body>\n"
                <<  "</html>\n";
    } else {
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "text/plain");
        beast::ostream(response_.body()) << "File not found\r\n";
    }
}

// Asynchronously transmit the response message.
void http_connection::write_response() {
    auto self = shared_from_this();
    response_.content_length(response_.body().size());
    http::async_write(
            socket_,
            response_,
            [self](beast::error_code ec, std::size_t) {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            });
}

// Check whether we have spent enough time on this connection.
void http_connection::check_deadline() {
    auto self = shared_from_this();
    deadline_.async_wait(
            [self](beast::error_code ec){
                if (!ec) {
                    // Close socket to cancel any outstanding operation.
                    self->socket_.close(ec);
                }
            });
}

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, std::deque<Transaction> *tx_deque) {
    acceptor.async_accept(socket,[&, tx_deque](beast::error_code ec) {
                              if (!ec)
                                  std::make_shared<http_connection>(std::move(socket))->start(tx_deque); // start - http_connection
                              http_server(acceptor, socket, tx_deque);
                          });
}

int Server::start_server(std::deque<Transaction> *tx_deque) {
    try {
        std::string ip_address = LOCAL_IP;
        auto const address = net::ip::make_address(ip_address);
        uint16_t port = PORT;
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket, tx_deque);
        std::cout << "Server has been started" << std::endl;
        ioc.run();
    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
