//
// Created by Alexander Syrgulev on 18.06.2022.
//

#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include "nlohmann/json.hpp"
#include "../Blockchain_core/Hex.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 49000

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using request_body_t = boost::beast::http::string_body;

class http_connection : public std::enable_shared_from_this<http_connection>{
public:
    explicit http_connection(tcp::socket socket):socket_(std::move(socket)){}
    // Initiate the asynchronous operations associated with the connection.
    void start();

private:
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
    void read_request();
    static bool valid_token_name(const std::string &token_name);
    void process_request();
    void create_response();
    void write_response();
    void check_deadline();
};

class Server {
public:
    static int start_server();
};


#endif //UVM_SERVER_H
