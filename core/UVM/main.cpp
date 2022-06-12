#include <assert.h>
#include <string.h>
#include "rocksdb/db.h"
#include <iostream>
#include <nlohmann/json.hpp>

#include <thread>
#include "vector"
#include "Blockchain_core/Crypto/kec256.h"
#include "Blockchain_core/Hex.h"
#include "Blockchain_core/Transaction.h"
#include "Blockchain_core/Block.h"

using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
#define kDBPath "/tmp/testdb"

int main(){
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
//    std::string key2 = "test_key";
//    std::string value = "test_value";
//    std::string fetched_value;
//    status = db->Put(rocksdb::WriteOptions(), "key1", value);
//    status = db->Get(rocksdb::ReadOptions(), "key1", &fetched_value);
//    std::cout << fetched_value;
//    delete db;

    std::map<std::string, std::string> map = {{"name", "unit"}, {"value", "1"}};
    Transaction tx = Transaction("g2px1", "gosha", 1,  map, "0", 1000);
    tx.generate_tx_hash();
    std::cout << "Transaction: " << tx << std::endl << tx.hash << std::endl;
    Block block = Block(0, 1, "0");
    block.push_tx(tx);
    block.set_current_date();
    block.generate_hash();
    std::cout << "Block: " << block << std::endl << block.hash;
    return 0;
}
