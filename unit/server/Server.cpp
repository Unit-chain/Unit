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

namespace my_program_state {
    std::size_t request_count() {
        static std::size_t count = 0;
        return ++count;
    }
    std::time_t now() {
        return std::time(0);
    }
}

class http_connection : public std::enable_shared_from_this<http_connection> {
public:
    http_connection(tcp::socket socket) : socket_(std::move(socket)){}
    // Initiate the asynchronous operations associated with the connection.
    void start(unit::list<ValidTransaction> *tx_deque) {
        this->tx_deque = tx_deque;
        read_request();
        check_deadline();
    }

private:
    // pointer to tx deque
    unit::list<ValidTransaction> *tx_deque;
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
    void read_request() {
        auto self = shared_from_this();
        http::async_read(
                socket_,
                buffer_,
                request_,
                [self](beast::error_code ec, std::size_t bytes_transferred){
                    boost::ignore_unused(bytes_transferred);
                    if (!ec) self->process_request();
                });
    }

    /*request*/
    void process_request() {
        response_.version(request_.version());
        response_.keep_alive(true);

        // featured json body
        boost::json::value body_to_json;
        boost::json::error_code ec;
        switch (request_.method()) {
            case http::verb::get:
                create_success_response(R"({"message":"Ok"})");
                break;
            case http::verb::post:
                body_to_json = boost::json::parse(request_.body(), ec);
                // parsing failed
                if (ec.failed()) {
                    create_error_response(R"({"message":"Failed to parse request"})");
                    break;
                }
                // passing success
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
    void create_error_response(std::string message = R"({"message":"Error"})", bool isJSON = true) {
        response_.result(http::status::bad_request);
        response_.set(http::field::content_type, (isJSON ? "application/json" : "text/plain"));
        response_.set(http::field::server, "Unit");
        beast::ostream(response_.body()) << message;
    }
    void create_success_response(std::string message = R"({"message":"Ok"})", bool isJSON = true) {
        response_.result(http::status::ok);
        response_.set(http::field::content_type, (isJSON ? "application/json" : "text/plain"));
        response_.set(http::field::server, "Unit");
        beast::ostream(response_.body()) << message;
    }
    void write_response() {
        auto self = shared_from_this();
        response_.content_length(response_.body().size());
        http::async_write(
                socket_,
                response_,
                [self](beast::error_code ec, std::size_t){
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                    self->deadline_.cancel();
                });
    }
    void check_deadline() {
        auto self = shared_from_this();
        deadline_.async_wait(
                [self](beast::error_code ec){
                    if (!ec) {
                        self->socket_.close(ec);
                    }
                });
    }
    /* END OF RESPONSES */
    /*------------------*/

    /*INSTRUCTIONS*/
    /*------------*/
    void process_instruction(boost::json::value json) {
        try {
            std::string instruction;
            instruction = boost::json::value_to<std::string>(json.at("instruction"));
            if (instruction == "i_balance") {
                i_balance(json);
            } else if (instruction == "i_push_transaction") {
                try {
                    i_push_transaction(json);
                } catch (std::exception &e) {
                    create_error_response(R"({"message":"Invalid data"})");
                    return;
                }
            } else if (instruction == "i_block_height") {
                i_block_height();
            } else if (instruction == "i_tx") {
                i_tx(json);
            } else if (instruction == "i_pool_size") {
                i_pool_size();
            } else {
                create_error_response(R"({"message":"Instruction not found"})");
            }
        } catch (const boost::wrapexcept<std::out_of_range> &o) {
            create_error_response(R"({"message":"Instruction field not found"})");
        }
    }
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, unit::list<Transaction> *tx_deque) {
    acceptor.async_accept(socket,
                          [&, tx_deque](beast::error_code ec){
                              if (!ec) std::make_shared<http_connection>(std::move(socket))->start(tx_deque);
                              http_server(acceptor, socket, tx_deque);
                          });
}

int Server::start_server(unit::list<Transaction> *tx_deque) {
// http_connection::initialize_instructions();
    rerun_server:{};
    try {
        std::string ip_address = LOCAL_IP;
        auto const address = net::ip::make_address(ip_address);
        uint16_t port = PORT;
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket, tx_deque);
        std::cout << "server has been started" << std::endl;
        ioc.run();
    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        goto rerun_server;
    }
    return 0;
}