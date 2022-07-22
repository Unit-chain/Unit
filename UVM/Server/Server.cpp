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

/*
 * type 0 = unit transfer
 * type 1 = token transfer
 * type 2 = create token
 * -----------------------
 * if type == 0, we need to check if transaction's field "to" not equals to field "from" and "amount" not 0 or null (DONE)
 * if type == 1, we need to check if extradata is valid for transferring tokens: name equals to token name and value equals to amount of transffering tokens (DONE)
 * if type == 2, we need to check if extradata's bytecode can be parsed to json(try-catch) and contains fields: supply and name (DONE)
 */

bool http_connection::push_transaction(std::string &transaction) {
    nlohmann::json transaction_json;
    try {
        transaction_json = nlohmann::json::parse(transaction);
    }
    catch (std::exception &e) {
        return false;
    }

    if (!transaction_json.contains("extradata") || !transaction_json["extradata"].contains("name") ||
        !transaction_json["extradata"].contains("value") || !transaction_json["extradata"].contains("bytecode"))
        return false;
    if (transaction_json["extradata"]["name"].empty() || transaction_json["extradata"]["value"].empty() ||
        transaction_json["extradata"]["bytecode"].empty())
        return false;

    auto extradata_name = transaction_json["extradata"]["name"].get<std::string>();
    auto extradata_value = transaction_json["extradata"]["value"].get<std::string>();
    auto extradata_bytecode = transaction_json["extradata"]["bytecode"].get<std::string>();
    std::map<std::string, std::string> map = {{"name",     extradata_name},
                                              {"value",    extradata_value},
                                              {"bytecode", extradata_bytecode}};
    auto from = transaction_json["from"].get<std::string>();
    auto to = transaction_json["to"].get<std::string>();
    Transaction tx = Transaction(from, to, transaction_json["type"].get<uint64_t>(), map, "0",
                                 transaction_json["amount"]);
    std::optional<std::string> op_balance = unit::DB::get_balance(tx.from);
    if(op_balance->empty())
        return false;

//    nlohmann::json wallet = nlohmann::json::parse(op_balance.value());
//    if(wallet["amount"].get<double>() < tx.amount)
//        return false;

    this->tx_deque->push_back(tx);
    return true;
}

void http_connection::start(std::deque<Transaction> *deque) {
    this->tx_deque = deque;
    read_request();
    check_deadline();
}

nlohmann::json json_type_validator(nlohmann::json json) {
    nlohmann::json empty;
    if (json["data"].empty()) return empty;
    nlohmann::json data = json["data"];
    if (data["type"].empty())
        return empty;
    int type = data["type"];
    std::string bytecode;
    nlohmann::json tmp;
    nlohmann::json out;
    switch (type) {
        case 0:
            if (!data["amount"].empty() &&
                !data["to"].empty() &&
                !data["from"].empty() && (data["to"].get<std::string>() != data["from"].get<std::string>()) &&
                data["amount"] != 0)
                out = data;
            break;
        case 1:
            bytecode = data["extradata"]["bytecode"];
            std::cout << 111 << std::endl;
            tmp = nlohmann::json::parse(hex_to_ascii(bytecode));
            std::cout << 222 << std::endl;
            if (!tmp["name"].empty() && !tmp["supply"].empty())
                out = tmp;
            break;
        case 2:
            if (!data["extradata"]["value"].empty() &&
                !data["amount"].empty() && !data["extradata"]["name"].empty())
                out = data;
            break;
        default:
            out = out;
            break;
    }
    return out;
}

void http_connection::bad_response(const std::runtime_error& e) {
    response_.result(http::status::not_found);
    nlohmann::json out = {{"Error",  "true"},
                          {"Result", e.what()}};
    beast::ostream(response_.body()) << to_string(out);
}

// Asynchronously receive a complete request message.
void http_connection::read_request() {
    auto self = shared_from_this();
    http::async_read(

            socket_,
            buffer_,
            request_,
            [self](beast::error_code ec,
                   std::size_t bytes_transferred) {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->process_request();
            });
}

void http_connection::good_response(std::string message) {
    nlohmann::json out = {{"Error",  "false"},
                          {"Result", message}};
    beast::ostream(response_.body()) << to_string(out);
}

// Determine what needs to be done with the request message.
void http_connection::process_request() {
    response_.version(request_.version());
    response_.keep_alive(true);
    nlohmann::json json;
    response_.set(http::field::server, "Unit");
    nlohmann::json out;
    http_connection::instructions instruction;
    switch (request_.method()) {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Unit");
            this->create_response();
            break;
        case http::verb::post:
            response_.set(http::field::content_type, "application/json");
            response_.set(http::field::server, "Unit");
            try {
                json = nlohmann::json::parse(request_.body());
            }
            catch (std::exception &e) {
                bad_response(std::runtime_error("error with parsing json"));
                write_response();
                break;
            }
            instruction = matchInstruction(json);
            if (!instruction_run(instruction, json)) {
                bad_response(std::runtime_error("error with instruction"));
                break;
            }
            break;
        default:
            bad_response(std::runtime_error("Invalid request-method" + std::string(request_.method_string())));
            break;
    }
    write_response();
}

bool http_connection::instruction_run(http_connection::instructions instruction, nlohmann::json json) {
    std::string out;
    switch (instruction) {
        case _false:
            return false;
        case i_balance:
            if (json["data"]["name"].empty())
                return false;
            i_balance_(json);
            return true;
        case i_push_transaction:
            out = json_type_validator(json).dump();
            good_response(push_transaction(out) ? "true" : "false");
            return true;
        case i_chainId:
            good_response(std::to_string(i_chainId_(json)));
            return true;
        case i_destruct:
            return false;
    }
    return false;
}

static std::map<std::string, http_connection::instructions> mapStringInstructions;

void http_connection::initialize_instructions() {
    mapStringInstructions["i_balance"] = instructions::i_balance;
    mapStringInstructions["i_chainid"] = instructions::i_chainId;
    mapStringInstructions["i_destruct"] = instructions::i_destruct;
    mapStringInstructions["_false"] = instructions::_false;
    mapStringInstructions["i_push_transaction"] = instructions::i_push_transaction;
}

http_connection::instructions http_connection::matchInstruction(nlohmann::json json) {
    if (json["instruction"].empty())
        return _false;
    auto instruction = mapStringInstructions[json["instruction"]];
    switch (instruction) {
        case _false:
            return _false;
        case i_balance:
            return i_balance;
        case i_chainId:
            return i_chainId;
        case i_push_transaction:
            return i_push_transaction;
        default:
            return _false;
    }
}

int http_connection::i_chainId_(nlohmann::json json) {
    // std::optional<std::string> height = unit::DB::get_block_height();
    return 1000000l;
}

void http_connection::i_balance_(nlohmann::json json) {
    std::string name = json["data"]["name"].get<std::string>();
    std::optional<std::string> op_balance = unit::DB::get_balance(name);
    if (!op_balance.has_value())
        beast::ostream(response_.body()) << R"({"balance": null})";
    else
        beast::ostream(response_.body()) << R"({"balance": )" << op_balance.value() << "}";
}

void http_connection::create_response() {
    if (request_.target() == "/count") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
                << "<html>\n"
                << "<head><title>Request count</title></head>\n"
                << "<body>\n"
                << "<h1>Request count</h1>\n"
                << "<p>There have been "
                << my_program_state::request_count()
                << " requests so far.</p>\n"
                << "</body>\n"
                << "</html>\n";
    } else if (request_.target() == "/time") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
                << "<html>\n"
                << "<head><title>Current time</title></head>\n"
                << "<body>\n"
                << "<h1>Current time</h1>\n"
                << "<p>The current time is "
                << my_program_state::now()
                << " seconds since the epoch.</p>\n"
                << "</body>\n"
                << "</html>\n";
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
            [self](beast::error_code ec) {
                if (!ec) {
                    // Close socket to cancel any outstanding operation.
                    self->socket_.close(ec);
                }
            });
}

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, std::deque<Transaction> *tx_deque) {
    acceptor.async_accept(socket, [&, tx_deque](beast::error_code ec) {

        if (!ec)
            std::make_shared<http_connection>(std::move(socket))->start(tx_deque); // start - http_connection
        http_server(acceptor, socket, tx_deque);
    });
}

int Server::start_server(std::deque<Transaction> *tx_deque) {
    http_connection::initialize_instructions();
    rerun_server:{};
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
    }
    catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        goto rerun_server;
    }
    return 0;
}