////
//// Created by Kirill Zhukov on 31.05.2022.
////
//
//#include "TXManager.h"
//
//TXManager::TXManager() {}
//
//TXManager::~TXManager() {}
//
//rocksdb::Status TXManager::openDB() {
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    options.error_if_exists = false;
//    return rocksdb::DB::Open(options, DATABASE_PATH, &db); // rocksdb::Status
//}
//
//void TXManager::pushTXToDB() {
//
//}
