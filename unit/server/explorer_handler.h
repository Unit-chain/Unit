//
// Created by Kirill Zhukov on 14.02.2023.
//

#ifndef UNIT_EXPLORER_HANDLER_H
#define UNIT_EXPLORER_HANDLER_H

#include "listener.h"
#include "shared_state.hpp"
#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>
#include "thread"
#include <ctime>
#include "thread"
#include "containers/vector.h"

[[noreturn]] static void writer(const boost::shared_ptr<listener> &listener, const boost::shared_ptr<unit::vector<std::string>> &queue) {
    time_t now = time(nullptr);
    printf("%lu: [INFO] --- auxiliary blockchain explorer-thread started\n", now);
    while(true) {
        if(queue->empty()) continue;
        listener->send(queue->back());
        queue->pop_back();
    }
}

static void start_explorer_server(boost::shared_ptr<unit::vector<std::string>> &queue, std::string addr = "0.0.0.0", int prt = 5555, int thrds = 1) {
    auto address = net::ip::make_address(addr);
    auto port = static_cast<unsigned short>(prt);
    auto doc_root = ".";
    auto const threads = thrds;

    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    auto lstnr = boost::make_shared<listener>(
            ioc,
            tcp::endpoint{address, port},
            boost::make_shared<shared_state>(doc_root));

    lstnr->run();

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
            [&ioc](boost::system::error_code const&, int)
            {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                ioc.stop();
            });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
                [&ioc]
                {
                    ioc.run();
                });
    std::thread th(writer, std::ref(lstnr), std::ref(queue));
    th.detach();
    ioc.run();

    // (If we get here, it means we got a SIGINT or SIGTERM)

    // Block until all the threads exit
    for(auto& t : v)
        t.join();
}

#endif //UNIT_EXPLORER_HANDLER_H
