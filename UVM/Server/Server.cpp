//
// Created by Gosha Stolyarov on 28.07.2022.
//
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/algorithm/hex.hpp>

#include <chrono>
#include <cstdlib>
#include <optional>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include "deque"

#include "Server.h"

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

namespace my_program_state
{
    std::size_t
    request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t
    now()
    {
        return std::time(0);
    }
}

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection(tcp::socket socket) : socket_(std::move(socket)){}

    // Initiate the asynchronous operations associated with the connection.
    void start(unit::list<Transaction> *tx_deque)
    {
        this->tx_deque = tx_deque;
        read_request();
        check_deadline();
    }

private:
    // pointer to tx deque
//    std::vector<Transaction> *tx_deque;
    unit::list<Transaction> *tx_deque;
    // The socket for the currently connected client.
    tcp::socket socket_;
    // The buffer for performing reads.
    beast::flat_buffer buffer_{8192};
    // The request message.
    http::request<http::string_body> request_;
    // The response message.
    http::response<http::dynamic_body> response_;
    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};
    // Asynchronously receive a complete request message.
    void read_request()
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

    /*request*/
    void process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(true);

        // featured json body
        boost::json::value body_to_json;

        boost::json::error_code ec;

        switch (request_.method())
        {
            case http::verb::get:

                create_success_response(R"({"message":"Ok"})");

                break;
            case http::verb::post:

                /* parsing options not implemented (no overload for function parse) */
                /*
                boost::json::parse_options opt;
                opt.allow_comments = true;
                opt.allow_trailing_commas = true;
                */

                body_to_json = boost::json::parse(request_.body());

                // parsing failed
                if (ec)
                {
                    create_error_response(R"({"message":"Failed to parse JSON"})");
                    break;
                }

                // pasing success
                process_instruction(body_to_json);

                break;
            default:
                create_error_response(R"({"message":"Invalid request type"})");
                break;
        }
        write_response();
    }

    /* RESPONSES */
    /*-----------*/
    void create_error_response(std::string message = R"({"message":"Error"})", bool isJSON = true)
    {
        response_.result(http::status::bad_request);
        response_.set(http::field::content_type, (isJSON ? "application/json" : "text/plain"));
        response_.set(http::field::server, "Unit");
        beast::ostream(response_.body()) << message;
    }
    void create_success_response(std::string message = R"({"message":"Ok"})", bool isJSON = true)
    {
        response_.result(http::status::ok);
        response_.set(http::field::content_type, (isJSON ? "application/json" : "text/plain"));
        response_.set(http::field::server, "Unit");
        beast::ostream(response_.body()) << message;
    }
    void write_response()
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
    void check_deadline()
    {
        auto self = shared_from_this();
        deadline_.async_wait(
                [self](beast::error_code ec)
                {
                    if (!ec)
                    {
                        self->socket_.close(ec);
                    }
                });
    }
    /* END OF RESPONSES */
    /*------------------*/

    /*INSTRUCTIONS*/
    /*------------*/
    void process_instruction(boost::json::value json)
    {
        try
        {
            std::string instruction;
            instruction = boost::json::value_to<std::string>(json.at("instruction"));
            if (instruction == "i_balance")
            {
                i_balance(json);
            }
            else if (instruction == "i_push_transaction")
            {
                try {
                    i_push_transaction(json);
                } catch (std::exception &e) {
                    create_error_response(R"({"message":"Invalid data"})");
                    return;
                }
            }
            else if (instruction == "i_block_height")
            {
                i_block_height();
            }
            else if (instruction == "i_tx")
            {
                i_tx(json);
            }
            else if (instruction == "i_pool_size"){
                i_pool_size();
            }
            else
            {
                create_error_response(R"({"message":"Instruction not found"})");
            }
        }
        catch (const boost::wrapexcept<std::out_of_range> &o)
        {
            create_error_response(R"({"message":"Instruction field not found"})");
        }
    }

    void i_balance(boost::json::value json)
    {
        try
        {
            std::string name;
            name = boost::json::value_to<std::string>(json.at("data").at("name"));
            std::optional<std::string> op_balance = unit::DB::get_balance(name);
            if (!op_balance.has_value())
                create_error_response(R"({"message":"Balance not found, address: )" + name + "\"}");
            else
                create_success_response(R"({"message":"Ok","balance":)" + op_balance.value() + "}");
        }
        catch (const boost::wrapexcept<std::out_of_range> &o)
        {
            create_error_response(R"({"message":"Invalid data"})");
        }
    }

    bool isEnoughTokenBalance(const boost::json::value& balance, const std::string& token_name, double value) {
        boost::json::object balance_json = balance.as_object();
        for(boost::json::array::iterator it = balance_json.at("tokens_balance").as_array().begin(); it != balance_json.at("tokens_balance").as_array().end(); ++it){
            if(it->as_object().contains(token_name)) {
                if(boost::json::value_to<double>(it->at(token_name)) < value)
                    return false;
                return true;
            }
        }
        return false;
    }

    bool isEnoughUnitBalance(const boost::json::value& balance, double value) {
        if(boost::json::value_to<double>(balance.at("amount")) < value)
            return false;
        return true;
    }

    void i_push_transaction(boost::json::value json)
    {
        try
        {
            std::string type;
            type = std::to_string(boost::json::value_to<int>(json.at("data").at("type")));
            if (type.length() == 0)
            {
                create_error_response(R"({"message":"'type' field is invalid"})");
                return;
            }

            std::string from;
            from = boost::json::value_to<std::string>(json.at("data").at("from"));
            if (from.length() == 0)
            {
                create_error_response(R"({"message":"'from' field is invalid"})");
                return;
            }

            std::optional<std::string> u_balance = unit::DB::get_balance(from);
            if (!u_balance.has_value())
            {
                create_error_response(R"({"message":"Balance not found, address: )" + from + "\"}");
                return;
            }

            if (type == "0")
            {
                std::string to;
                to = boost::json::value_to<std::string>(json.at("data").at("to"));
                if (to.length() == 0)
                {
                    create_error_response(R"({"message":"'to' field is invalid"})");
                    return;
                }

                std::string amount;
                amount = std::to_string(boost::json::value_to<double>(json.at("data").at("amount")));
                if (amount.length() == 0)
                {
                    create_error_response(R"({"message":"'amount' field is invalid"})");
                    return;
                }

                double d_amount;
                try
                {
                    d_amount = std::stod(amount);
                }
                catch (const std::exception &e)
                {
                    create_error_response(R"({"message":"Invalid amount"})");
                    return;
                }

                std::map<std::string, std::string> extradata = {{"name", "null"},
                                                                {"value", "null"},
                                                                {"bytecode", "null"}};

                std::optional<std::string> op_balance = unit::DB::get_balance(from);
                if(op_balance->empty()) {
                    std::string response = R"({"message":"Error occurred, please try again"})";
                    create_error_response(response);
                }

                if(!isEnoughUnitBalance(boost::json::parse(op_balance.value()), boost::json::value_to<double>(json.at("data").at("amount")))) {
                    std::string response = R"({"message":"Error occurred, please try again"})";
                    create_error_response(response);
                    return;
                }


                Transaction tx = Transaction(from, to, 0, json.at("data").at("extradata"), "0", d_amount);
                tx.generate_tx_hash();
                this->tx_deque->push_back(tx);
                std::string response = R"({"message":"Ok","hash":")" + tx.getHash() + R"("})";
                create_success_response(response);
                return;
            }
            else if (type == "1")
            {
                std::string bytecode;
                std::string decoded_bytecode;
                bytecode = boost::json::value_to<std::string>(json.at("data").at("extradata").at("bytecode"));
                if (bytecode.length() == 0)
                {
                    create_error_response(R"({"message":"'bytecode' field is invalid"})");
                    return;
                }
                try
                {
                    decoded_bytecode = boost::algorithm::unhex(bytecode);
                }
                catch (const boost::algorithm::hex_decode_error &e)
                {
                    create_error_response(R"({"message":"Bytecode decoding error"})");
                    return;
                }

                boost::json::error_code ec;
                boost::json::value bytecode_to_json = boost::json::parse(decoded_bytecode, ec);
                // parsing failed
                if (ec)
                {
                    create_error_response(R"({"message":"Failed to parse bytecode to JSON"})");
                    return;
                }

                std::string name;
                std::string supply;
                try
                {
                    name = boost::json::value_to<std::string>(bytecode_to_json.at("name"));
                    supply = std::to_string(boost::json::value_to<double>(bytecode_to_json.at("supply")));
                }
                catch (const boost::wrapexcept<std::out_of_range> &e)
                {
                    create_error_response(R"({"message":"Not enough fields in bytecode data"})");
                    return;
                }

                if (name.length() == 0)
                {
                    create_error_response(R"({"message":"Bytecode error: 'name' is empty"})");
                    return;
                }
                if (supply.length() == 0)
                {
                    create_error_response(R"({"message":"Bytecode error: 'supply' is empty"})");
                    return;
                }

                try
                {
                    double r = std::stod(supply);
                }
                catch (const std::exception &e)
                {
                    create_error_response(R"({"message":"Bytecode error: 'supply' is not a number"})");
                    return;
                }

//                std::map<std::string, std::string> extradata = {{"name", "null"},
//                                                                {"value", "null"},
//                                                                {"bytecode", bytecode}};

                Transaction tx = Transaction(from, "", 1, json.at("data").at("extradata"), "0", 0);
                tx.generate_tx_hash();
                this->tx_deque->push_back(tx);
                std::string response = R"({"message":"Ok","hash":")" + tx.getHash() + R"("})";
                create_success_response(response);
                return;
            }
            else if (type == "2")
            {
                std::string to;
                to = boost::json::value_to<std::string>(json.at("data").at("to"));
                if (to.length() == 0)
                {
                    create_error_response(R"({"message":"'to' field is invalid"})");
                    return;
                }

                std::string name;
                name = boost::json::value_to<std::string>(json.at("data").at("extradata").at("name"));
                if (name.length() == 0)
                {
                    create_error_response(R"({"message":"'name' field is invalid"})");
                    return;
                }

                std::string value;
                value = boost::json::value_to<std::string>(json.at("data").at("extradata").at("value"));
                if (value.length() == 0)
                {
                    create_error_response(R"({"message":"'value' field is invalid"})");
                    return;
                }

                double d_value;
                try
                {
                    d_value = std::stod(value);
                }
                catch (const std::exception &e)
                {
                    create_error_response(R"({"message":"'value' field is not a number"})");
                    return;
                }

                std::map<std::string, std::string> extradata = {{"name", name},
                                                                {"value", value},
                                                                {"bytecode", "null"}};

                std::optional<std::string> op_balance = unit::DB::get_balance(from);
                if(op_balance->empty()) {
                    std::string response = R"({"message":"Error occurred, please try again"})";
                    create_error_response(response);
                }

                if(!isEnoughTokenBalance(boost::json::parse(op_balance.value()), boost::json::value_to<std::string>(json.at("data").at("extradata").at("name")), std::stod(boost::json::value_to<std::string>(json.at("data").at("extradata").at("value"))))) {
                    std::string response = R"({"message":"Low balance"})";
                    create_error_response(response);
                    return;
                }

                Transaction tx = Transaction(from, to, 2, json.at("data").at("extradata"), "0", 0);
                tx.generate_tx_hash();
                this->tx_deque->push_back(tx);
                std::string response = R"({"message":"Ok","hash":")" + tx.getHash() + R"("})";
                create_success_response(response);
                return;
            }
            create_error_response(R"({"message":"No such type"})");
        }
        catch (const boost::wrapexcept<std::out_of_range> &o)
        {
            create_error_response(R"({"message":"Data is invalid"})");
        }
    }

    void i_block_height()
    {
        std::optional<std::string> block_height = unit::DB::get_block_height();
        if (!block_height.has_value())
            create_error_response();
        else
            create_success_response(R"({"message":"Ok","block_height":)" + block_height.value() + "}");
    }
    void i_pool_size()
    {
        create_success_response(R"({"message":"Ok","pool_size":)" + std::to_string(this->tx_deque->size()) + "}");
    }

    void i_tx(boost::json::value json)
    {
        try
        {
            std::string hash;
            hash = boost::json::value_to<std::string>(json.at("data").at("hash"));
            std::optional<std::string> op_tx = unit::DB::find_transaction(hash);
            if (!op_tx.has_value())
                create_error_response(R"({"message":"Transaction not found"})");
            else
                create_success_response(R"({"message":"Ok","transaction":)" + op_tx.value() + "}");
        }
        catch (const boost::wrapexcept<std::out_of_range> &o)
        {
            create_error_response(R"({"message":"Invalid data"})");
        }
    }
    /*END OF INSTRUCTIONS*/
    /*-------------------*/
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, unit::list<Transaction> *tx_deque)
{
    acceptor.async_accept(socket,
                          [&, tx_deque](beast::error_code ec)
                          {
                              if (!ec)
                                  std::make_shared<http_connection>(std::move(socket))->start(tx_deque);
                              http_server(acceptor, socket, tx_deque);
                          });
}

int Server::start_server(unit::list<Transaction> *tx_deque)
{
// http_connection::initialize_instructions();
    rerun_server:
    {
    };
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
        goto rerun_server;
    }
    return 0;
}