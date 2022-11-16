//
// Created by Gosha Stolyarov on 28.07.2022.
//
#ifndef UVM_SERVER_H
#define UVM_SERVER_H

#include "RpcMethod.h"
#include "ServerIncludes.h"
#include "RpcFilterChain.h"
#include "RpcFilterBuilder.h"
#include "../libdevcore/db/DB.h"
#include "../pools/TransactionPool.h"
#include "../libdevcore/db/DBProvider.h"
#include "../libdevcore/datastructures/containers/list.h"
#include "../libdevcore/datastructures/containers/vector.h"
#include "../libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"

#define LOCAL_IP "127.0.0.1"
#define PORT 29000

class Server {
public:
    static int start_server(TransactionPool *transactionPool, std::string &userDBPath,
                            std::string &historyPath, std::string &blockPath, std::string &transactionPath, Block *last);
    static bool isEnoughTokenBalance(const boost::json::value& balance, const std::string& token_name, double value);
    static bool isEnoughUnitBalance(const boost::json::value& balance, double value);
};


#endif //UVM_SERVER_H