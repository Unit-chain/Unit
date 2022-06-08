//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "TXManager.h"
#include "error_handling/result.h"
#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_simple_example";
#else
std::string kDBPath = "/tmp/rocksdb_simple_example";
#endif

TXManager::TXManager() {}

TXManager::~TXManager() {}

rocksdb::Status TXManager::openDB() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
    return status;
}

void TXManager::pushTXToDB(const std::string& tx_raw) {
    nlohmann::json txJSON = nlohmann::json::parse(tx_raw);
    return;
}
