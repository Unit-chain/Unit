//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "Blockchain_db.h"

template<class T>
void Blockchain_db<T>::start_node_db() {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;
    rocksdb::DB* db;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
    assert(status.ok());
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
    assert(status.ok());
    delete db;
}

//template<class T>
//Result<std::string> Blockchain_db<T>::push_block(Block &block) {
//    rocksdb::Options options;
//    options.create_if_missing = false;
//    options.error_if_exists = false;
//    rocksdb::DB* db;
//    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
//    return Result<std::string>(__1::basic_string());
//}


