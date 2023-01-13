//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#include "RpcMethod.h"
#include "ServerIncludes.h"
#include "RpcFilterChain.h"
#include "TransactionPool.h"
#include "PendingPool.h"
#include "containers/list.h"
#include "block/Block.h"
#include "containers/vector.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 29000

class Server {
public:
    static int start_server(TransactionPool *transactionPool, PendingPool *pool, std::string &userDBPath, std::string &historyPath, std::string &blockPath, std::string &transactionPath);
};


class http_connection : public std::enable_shared_from_this<http_connection> {
public:
    http_connection(tcp::socket socket) : socket_(std::move(socket)){}
//    http_connection(tcp::socket socket) : socket_(std::move(socket)){}
    // Initiate the asynchronous operations associated with the connection.
    void start(TransactionPool *transactionPool, PendingPool *pool, const std::string &userDBPath, const std::string &historyPath, const std::string &blockPath, const std::string &transactionPath) {
        this->transactionPool = transactionPool;
        this->userProvider = unit::BasicDB(userDBPath);
        this->historyProvider = unit::BasicDB(historyPath);
        this->blockDBProvider = unit::BasicDB(blockPath);
        this->transactionDBProvider = unit::BasicDB(transactionPath);
        this->pendingPool = pool;
        read_request();
        check_deadline();
    }

private:
    // database provider for users accounts
    unit::BasicDB userProvider{};
    // database provider for history accounts
    unit::BasicDB historyProvider{};
    // database provider for block DB
    unit::BasicDB blockDBProvider{};
    // database provider for transaction DB
    unit::BasicDB transactionDBProvider{};
    // pointer to tx pool
    TransactionPool *transactionPool;
    // pointer to tx pool
    PendingPool *pendingPool;
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
    /// const errors
    static constexpr char httpMethodError[106] = R"({"jsonrpc": "2.0", "error": {"code": -32003, "message": "HTTP request method not supported"}, "id": null})";
    static constexpr char parsingError[96] = R"({"jsonrpc": "2.0", "error": {"code": -32700, "message": "failed to parse request"}, "id": null})";
    static constexpr char emptyBalanceError[86] = R"({"jsonrpc": "2.0", "error": {"code": -32002, "message": "empty balance"}, "id": null})";
    static constexpr char defaultError[93] = R"({"jsonrpc": "2.0", "error": {"code": -32004, "message": "something went wrong"}, "id": null})";
    static constexpr char defaultAccountError[110] = R"({"jsonrpc": "2.0", "error": {"code": -32004, "message": "something happening with your account"}, "id": null})";
    static constexpr char badNonce[114] = R"({"jsonrpc": "2.0", "error": {"code": -32005, "message": "old nonce is used for current transaction"}, "id": null})";
    static constexpr char invalidSignature[90] = R"({"jsonrpc": "2.0", "error": {"code": -32006, "message": "invalid signature"}, "id": null})";
    static constexpr char invalidMethod[89] = R"({"jsonrpc": "2.0", "error": {"code": -32001, "message": "method not found"}, "id": null})";


    inline void read_request() {
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
    inline void process_request() {
        response_.version(request_.version());
        response_.keep_alive(true);
        // featured json body
        boost::json::value body_to_json;
        boost::json::error_code ec;
        switch (request_.method()) {
            case http::verb::get:
                create_error_response(http_connection::httpMethodError);
                break;
            case http::verb::post:
                body_to_json = boost::json::parse(request_.body(), ec);
                if (ec.failed()) {
                    create_error_response(http_connection::parsingError);
                    break;
                }
                process_instruction(body_to_json);
                break;
            default:
                create_error_response(http_connection::httpMethodError);
                break;
        }
        write_response();
    }

    /* RESPONSES */
    /*-----------*/
    inline void create_error_response(const std::string& message = R"({"message":"Error"})") {
        response_.result(http::status::bad_request);
        response_.set(http::field::content_type, "application/json");
        response_.set(http::field::server, "unit");
        beast::ostream(response_.body()) << message;
    }

    inline void write_response() {
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
    inline void check_deadline() {
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

    inline void process_instruction(boost::json::value& json) {
        try {
            auto method = boost::json::value_to<std::string>(json.at("method"));
            RpcFilterChain rpcFilterChain{};
            if ("transfer" == method) {
                rpcFilterChain.add(new BasicTransactionRpcFilter(&(this->userProvider),&(this->response_), this->transactionPool));
            } else if ("unit_get_balance" == method) {
                rpcFilterChain.add(new BasicBalanceFilter(&(this->userProvider),&(this->response_)));
            } else if ("unit_get_tx_pool_size" == method) {
                rpcFilterChain.add(new BasicPoolFilter(&(this->response_), this->transactionPool));
            } else if ("unit_get_address_tx_history" == method) {
                rpcFilterChain.add(new BasicBalanceHistoryFilter(&(this->response_), &(this->historyProvider)));
            } else if ("unit_get_block_height" == method) {
                rpcFilterChain.add(new BasicBlockHeightFilter(&(this->response_), &this->blockDBProvider));
            } else if ("unit_get_tx" == method) {
                rpcFilterChain.add(new BasicTransactionByHashRpcFilter(&(this->response_), &(this->transactionDBProvider)));
            } else {
                create_error_response(rpcError::invalidMethod);
                throw RpcMethodSupportException();
            }
            rpcFilterChain.filter(&json);
        } catch (const std::exception &e) {
            std::cout << "instruction process error: " << e.what() << std::endl;
            if (this->response_.body().size() == 0) create_error_response(rpcError::defaultError);
        }
    }
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket, TransactionPool *transactionPool, PendingPool *pool,
                 const std::string &userDBPath, const std::string &historyPath, const std::string &blockPath, const std::string &transactionPath) {
    acceptor.async_accept(socket,
                          [&, transactionPool, userDBPath, historyPath, transactionPath](beast::error_code ec){
                              if (!ec) std::make_shared<http_connection>(std::move(socket))->start(transactionPool, pool, userDBPath, historyPath, blockPath, transactionPath);
                              http_server(acceptor, socket, transactionPool, pool, userDBPath, historyPath, blockPath, transactionPath);
                          });
}

int Server::start_server(TransactionPool *transactionPool, PendingPool *pool, std::string &userDBPath,
                         std::string &historyPath, std::string &blockPath, std::string &transactionPath) {
    rerun_server:;
    try {
        std::string ip_address = LOCAL_IP;
        auto const address = net::ip::make_address(ip_address);
        uint16_t port = PORT;
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket, transactionPool, pool, userDBPath, historyPath, blockPath, transactionPath);
        std::cout << "server started" << std::endl;
        auto const threads = (int) std::thread::hardware_concurrency() / 2;
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i)
            v.emplace_back(
                    [&ioc] {
                        ioc.run();
                    });
        ioc.run();
        for(auto& t : v) t.join();
        ioc.run();
    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        goto rerun_server;
    }
    return 0;
}

#endif //UVM_SERVER_H