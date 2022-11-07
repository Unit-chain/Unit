//
// Created by Gosha Stolyarov on 28.07.2022.
//
#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include "RpcMethod.h"
#include "../libdevcore/datastructures/containers/list.h"
#include "../libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"
#include "../libdevcore/db/DBProvider.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 29000

class Server {
public:
    static int start_server(unit::list<ValidTransaction> *tx_deque, const std::shared_ptr<std::string>& path);
    static bool isEnoughTokenBalance(const boost::json::value& balance, const std::string& token_name, double value);
    static bool isEnoughUnitBalance(const boost::json::value& balance, double value);
};


#endif //UVM_SERVER_H