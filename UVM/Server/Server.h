//
// Created by Gosha Stolyarov on 28.07.2022.
//
#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include "../Blockchain_core/Transaction.h"
#include "../Blockchain_core/Hex.h"
#include "../Blockchain_core/DB/DB.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 49000

class Server {
public:
    static int start_server(std::vector<Transaction> *tx_deque);
};


#endif //UVM_SERVER_H