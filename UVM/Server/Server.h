//
// Created by Gosha Stolyarov on 28.07.2022.
//
#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include "../Blockchain_core/Transaction.h"
#include "../Blockchain_core/Hex.h"
#include "../Blockchain_core/DB/DB.h"
#include "../containers/list.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 29000

class Server {
public:
    static int start_server(unit::list<Transaction> *tx_deque);
    static bool isEnoughTokenBalance(const boost::json::value& balance, const std::string& token_name, double value);
    static bool isEnoughUnitBalance(const boost::json::value& balance, double value);
};


#endif //UVM_SERVER_H