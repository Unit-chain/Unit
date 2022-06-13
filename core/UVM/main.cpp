#include "rocksdb/db.h"
#include <iostream>
#include <nlohmann/json.hpp>

#include "vector"
#include "Blockchain_core/Transaction.h"
#include "Blockchain_core/DB/Blockchain_db.h"

using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;

int main(){
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    rocksdb::Status status = rocksdb::DB::Open(options, "/Users/kirillzhukov/Documents/unit_db", &db);
//    assert(status.ok());
//    std::string key2 = "test_key";
//    std::string value = "test_value";
//    std::string fetched_value;
////    status = db->Put(rocksdb::WriteOptions(), "key1", value);
//    status = db->Get(rocksdb::ReadOptions(), "key1", &fetched_value);
//    std::cout << fetched_value;
//    delete db;




//    std::map<std::string, std::string> map = {{"name", "unit"}, {"value", "1"}};
//    Transaction tx = Transaction("g2px1", "gosha", 0,  map, "0", 1000);
//    tx.generate_tx_hash();
//    std::cout << "Transaction: " << tx << std::endl << tx.hash << std::endl;
//    Blockchain_db blockchainDb = Blockchain_db();
//    Result<bool> result = blockchainDb.push_transaction(tx);
//    std::cout << result.ok() << std::endl;


//    std::map<std::string, std::string> map = {{"name", "carrot"}, {"value", "100"}};
//    Transaction tx = Transaction("g2px1", "gosha", 1,  map, "0", 0);
//    tx.generate_tx_hash();
//    std::cout << "Transaction: " << tx << std::endl << tx.hash << std::endl;
//    Blockchain_db blockchainDb = Blockchain_db();
//    Result<bool> result = blockchainDb.push_transaction(tx);
//    std::cout << result.ok() << std::endl;


    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;
    options.create_missing_column_families = true;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    rocksdb::DB* db;
    Blockchain_db blockchainDb = Blockchain_db();
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, blockchainDb.columnFamilies, &handles, &db);
    std::string value;
    db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice("gosha"), &value);
    std::cout << value;
    return 0;
}
