////
//// Created by Kirill Zhukov on 31.05.2022.
////
//
//#include <libc.h>
//#include "TXManager.h"
//#include "error_handling/Result.h"
//#include "ENV/env.h"
//
//TXManager::TXManager() {}
//
//TXManager::~TXManager() {}
//
//rocksdb::Status TXManager::openDB_W() {
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    options.error_if_exists = false;
//    options.IncreaseParallelism(cpus);
//    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
//    return status;
//}
//
//void TXManager::pushTXToDB(const std::string& tx_raw) {
//    nlohmann::json txJSON = nlohmann::json::parse(tx_raw);
//    return;
//}
//
//rocksdb::Status TXManager::openDB_RO() {
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    options.error_if_exists = false;
//    options.IncreaseParallelism(cpus);
//    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, kDBPath, &db);
//    return status;
//}
