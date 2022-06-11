////
//// Created by Kirill Zhukov on 10.06.2022.
////
//
//#include "Block.h"
//#include "../TXManager.h"
//#include "../ENV/env.h"
//
//std::string Block::previous_block_num() {
//    rocksdb::Status s = TXManager::openDB_RO();
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, kDBPath, &db);
//    if(status.ok()) ;
//    return std::string();
//}
//
//std::ostream &operator<<(std::ostream &out, const Block &block) {
//    return out << "Block{" << block.hash << ", " << block.prev_hash << ", " << block.net_version << ", " << block.index << "}";
//}
//
//Block::~Block() {}
