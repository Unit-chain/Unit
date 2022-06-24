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
#include "deque"
#include <string>
#include "nlohmann/json.hpp"
#include "../Blockchain_core/Transaction.h"
#include "../Blockchain_core/Hex.h"
#include "../Blockchain_core/DB/DB.h"

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
    void start(std::deque<Transaction> *deque);
    enum instructions { _false,
        i_balance,
        i_chainId,
        create,
        destruct
    };
//    static std::map<std::string, instructions> mapStringInstructions;
    static void matchInstruction(const http::response<http::dynamic_body>, nlohmann::json);
    static void initialize_instructions();




private:
    std::deque<Transaction> *tx_deque;
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
    static void i_chainId_(http::response<http::dynamic_body>, nlohmann::json);
    static void i_balance_(http::response<http::dynamic_body>, nlohmann::json);
    void read_request();
//    static bool valid_token_name(const std::string &token_name);
    void process_request();
    void create_response();
//    void create_json_response();
    void write_response();
    void check_deadline();
    bool push_transaction(std::string &transaction);
};


class Server {
public:
    static int start_server(std::deque<Transaction> *tx_deque);
};


#endif //UVM_SERVER_H
