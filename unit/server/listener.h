//
// Created by Kirill Zhukov on 14.02.2023.
//

#ifndef UNIT_LISTENER_H
#define UNIT_LISTENER_H

#include <boost/smart_ptr.hpp>
#include "boost/beast.hpp"
#include <memory>
#include <string>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;

class shared_state;

class listener : public boost::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<shared_state> state_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec, tcp::socket socket);

public:
    listener(
            net::io_context& ioc,
            tcp::endpoint endpoint,
            boost::shared_ptr<shared_state> const& state);

    // Start accepting incoming connections
    void run();
    void send(std::string &message);
    bool isStateEmpty();
};
#endif //UNIT_LISTENER_H
