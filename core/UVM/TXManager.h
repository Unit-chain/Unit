//
// Created by Kirill Zhukov on 31.05.2022.
//

#ifndef UVM_TXMANAGER_H
#define UVM_TXMANAGER_H

#include <cassert>
#include "string"
#include "rocksdb/db.h"
#include "error_handling/Result.h"
#include <nlohmann/json.hpp>

class TXManager {
public:
    TXManager();
    virtual ~TXManager();
//    static void pushTXToDB(const std::string& tx_raw);
//    static rocksdb::Status openDB_W(); // read only database; faster than read-only database
//    static rocksdb::Status findTXByAccount(std::string &account);
//    static rocksdb::Status countBalanceByAccount(std::string &account);
//    static rocksdb::Status getUserTokens(std::string &account);
//    static rocksdb::Status countBalanceInTokens(std::string &account, std::string &token_name);
//    static rocksdb::Status openDB_RO(); // read only database; faster than read-write database
};

#endif //UVM_TXMANAGER_H
