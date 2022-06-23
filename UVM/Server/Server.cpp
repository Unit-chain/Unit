//
// Created by Alexander Syrgulev on 18.06.2022.
//

#include "Server.h"

namespace my_program_state
{
    std::size_t request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t now()
    {
        return std::time(0);
    }
}

bool http_connection::push_transaction(std::string &transaction)
{
    nlohmann::json transaction_json;
    try
    {
        transaction_json = nlohmann::json::parse(transaction);
    }
    catch (std::exception &e)
    {
        return false;
    }
    if (!transaction_json.contains("extradata") || !transaction_json["extradata"].contains("name") || !transaction_json["extradata"].contains("value") || !transaction_json["extradata"].contains("bytecode"))
        return false;
    if (transaction_json["extradata"]["name"].empty() || transaction_json["extradata"]["value"].empty() || transaction_json["extradata"]["bytecode"].empty())
        return false;
    auto extradata_name = transaction_json["extradata"]["name"].get<std::string>();
    auto extradata_value = transaction_json["extradata"]["value"].get<std::string>();
    auto extradata_bytecode = transaction_json["extradata"]["bytecode"].get<std::string>();
    std::map<std::string, std::string> map = {{"name", extradata_name}, {"value", transaction_json["extradata"]["value"].get<std::string>()}, {"bytecode", extradata_bytecode}};
    auto from = transaction_json["from"].get<std::string>();
    auto to = transaction_json["to"].get<std::string>();
    Transaction tx = Transaction(from, to, transaction_json["type"].get<uint64_t>(), map, "0", transaction_json["amount"]);
    // if (!unit::DB::validate_sender_balance(tx))
    //     return false;

    this->tx_deque->push_back(tx);
    return true;
}

void http_connection::start(std::deque<Transaction> *deque)
{
    this->tx_deque = deque;
    read_request();
    check_deadline();
}

Result<nlohmann::json> json_type_validator(int type, nlohmann::json data)
{
    std::string bytecode;
    nlohmann::json tmp;
    bool fl = false;
    nlohmann::json out;
    switch (type)
    {
    case 0:
        std::cout << "json_type_validator type 0\n";
        fl = (!data["amount"].empty() &&
              !data["to"].empty() &&
              !data["from"].empty() && (data["to"].get<std::string>() != data["from"].get<std::string>()) && data["amount"] != 0);
        std::cout << "fl = " << fl << "\n";
        out = data;
        break;
    case 1:
        std::cout << "json_type_validator type 1\n";
        fl = (!data["extradata"]["value"].empty() &&
              !data["amount"].empty() &&
              (data["extradata"]["value"].get<int>() == data["amount"].get<int>()) &&
              !data["extradata"]["name"].empty());
        std::cout << "fl = " << fl << "\n";
        out = data;
        break;
    case 2:
        std::cout << "json_type_validator type 2\n";
        bytecode = data["extradata"]["bytecode"];
        tmp = nlohmann::json::parse(hex_to_ascii(bytecode));
        fl = (!tmp["name"].empty() && !tmp["supply"].empty());
        std::cout << "fl = " << fl << "\n";
        out = tmp;
        break;
    default:
        std::cout << "json_type_validator invalid type\n";
        std::cout << "fl = " << fl << "\n";
        out = data;
        break;
    }
    return {out, fl};
}

void bad_resposne(http::response<http::dynamic_body> response)
{
    std::cout << "type = Unknown\n";
    beast::ostream(response.body()) << "false";
}

// Asynchronously receive a complete request message.
void http_connection::read_request()
{
    auto self = shared_from_this();
    http::async_read(
        socket_,
        buffer_,
        request_,
        [self](beast::error_code ec,
               std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            if (!ec)
                self->process_request();
        });
}

// Determine what needs to be done with the request message.
void http_connection::process_request()
{
    response_.version(request_.version());
    response_.keep_alive(true);
    std::string string_body_test;
    nlohmann::json json;
    response_.set(http::field::server, "Unit");
    nlohmann::json data;
    int type;
    Result<nlohmann::json> in("", false);
    switch (request_.method())
    {
    case http::verb::get:
        response_.result(http::status::ok);
        response_.set(http::field::server, "Unit");
        this->create_response();
        break;
    case http::verb::post:
        response_.set(http::field::content_type, "application/json");
        response_.set(http::field::server, "Unit");
        try
        {
            json = nlohmann::json::parse(request_.body());
            this->matchInstruction(response_, json);
            data = json["data"];
            type = data["type"];
        }
        catch (std::exception &e)
        {
            response_.result(http::status::not_found);
            nlohmann::json out = {{"Error", "true"},
                                  {"Result", e.what()}};
            beast::ostream(response_.body()) << to_string(out);
            break;
        }

        in = json_type_validator(type, data);
        if (in.get_fl())
        {
            std::string tx = in.get_value().dump();
//            beast::ostream(response_.body()) << push_transaction(tx);
        }
        else
        {
            bad_resposne(response_);
            break;
        }

        // TYPE 0
        // if (type == 0)
        // {
        //     // Checking first condition:
        //     // nullptr when no amount
        //     if ()
        //     {

        //
        //     else
        //     {
        //         bad_resposne(response_);
        //         break;
        //     }
        //     // TYPE 1
        // }
        // else if (type == 1)
        // {
        //     std::cout << "type = 1 (token transfer)\n";
        //     // Checking second condition:
        //     if ()
        //     {

        //         std::string tx = data.dump(0);
        //         beast::ostream(response_.body()) << push_transaction(tx);
        //     }
        //     else
        //     {
        //         bad_resposne(response_);
        //         break;
        //     }
        //     // TYPE 2
        // }
        // else if (type == 2)
        // {
        //     // std::cout << "type = 2 (create token)\n";
        //     // std::string bytecode = data["extradata"]["bytecode"];
        //     // nlohmann::json tmp = nlohmann::json::parse(hex_to_ascii(bytecode));
        //     // Checking third condition:
        //     if ()
        //     {
        //         std::cout << "name = " << tmp["name"] << "\tsupply = " << tmp["supply"] << "\n";
        //         std::string tx = data.dump(0);
        //         auto fl = push_transaction(tx);
        //         beast::ostream(response_.body()) << "push_transaction(tx)";
        //     }
        // }

        response_.result(http::status::ok);
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

static std::map<std::string, http_connection::instructions> mapStringInstructions;
void http_connection::initialize_instructions()
{
    mapStringInstructions["i_balance"] = instructions::i_balance;
    mapStringInstructions["i_chainid"] = instructions::i_chainId;
    mapStringInstructions["create"] = instructions::create;
    mapStringInstructions["destruct"] = instructions::destruct;
    mapStringInstructions["_false"] = instructions::_false;
    std::cout << "\ninstructions were initialized\n";
    std::cout << "s_mapStringValues contains "
              << mapStringInstructions.size()
              << " entries." << std::endl;
    for (auto it = mapStringInstructions.begin(); it != mapStringInstructions.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
}
void http_connection::matchInstruction(http::response<http::dynamic_body> response, nlohmann::json json)
{
    auto instructionString = json["instruction"];
    std::cout << "instructionString = " << instructionString << "\n";
    auto instruction = mapStringInstructions[instructionString];
    std::cout << "instruction = " << instruction << "\n";
    switch (mapStringInstructions[json["instruction"]])
    {
    case _false:
        std::cout << "match instruction: no match\n";
        break;
    case i_balance:
        std::cout << "i_balance match\n";
        i_balance_(response, json);
        break;
    case i_chainId:
        std::cout << "i_chainid match\n";
        i_chainId_(response, json);
        break;
    default:
        std::cout << "unknown match instruction\n";
        break;
    }
}

void http_connection::i_chainId_(http::response<http::dynamic_body> response, nlohmann::json json)
{
    // std::optional<std::string> height = unit::DB::get_block_height();
    beast::ostream(response.body()) << R"({"balance": )"
                                    << "'height'"
                                    << " !!!TEST!!!}";
}

void http_connection::i_balance_(http::response<http::dynamic_body> response, nlohmann::json json)
{
    auto name = json["data"]["name"].get<std::string>();
    // std::optional<std::string> op_balance = unit::DB::get_balance(name);
    // if (!op_balance.has_value())
    // beast::ostream(response.body()) << R"({"balance": null})";
    // else
    beast::ostream(response.body()) << R"({"balance": )"
                                    << "op_balance.value()"
                                    << "}";
}

void http_connection::create_response()
{
    if (request_.target() == "/count")
    {
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
    }
    else if (request_.target() == "/time")
    {
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
    }
    else
    {
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "text/plain");
        beast::ostream(response_.body()) << "File not found\r\n";
    }
}

// void http_connection::create_json_response() {
//     response_.result(ht)
// }

// Asynchronously transmit the response message.
void http_connection::write_response()
{
    auto self = shared_from_this();
    response_.content_length(response_.body().size());
    http::async_write(
        socket_,
        response_,
        [self](beast::error_code ec, std::size_t)
        {
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            self->deadline_.cancel();
        });
}

// Check whether we have spent enough time on this connection.
void http_connection::check_deadline()
{
    auto self = shared_from_this();
    deadline_.async_wait(
        [self](beast::error_code ec)
        {
            if (!ec)
            {
                // Close socket to cancel any outstanding operation.
                self->socket_.close(ec);
            }
        });
}

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, std::deque<Transaction> *tx_deque)
{
    acceptor.async_accept(socket, [&, tx_deque](beast::error_code ec)
                          {
        if (!ec)
            std::make_shared<http_connection>(std::move(socket))->start(tx_deque); // start - http_connection
        http_server(acceptor, socket, tx_deque); });
}

int Server::start_server(std::deque<Transaction> *tx_deque)
{
    http_connection::initialize_instructions();
    try
    {
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
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}