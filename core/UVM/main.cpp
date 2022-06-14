#include "rocksdb/db.h"
#include <iostream>
#include "strhash.h"
#include "sstream"
#include "Blockchain_core/Hex.h"
#include <nlohmann/json.hpp>

#include "vector"
#include "Blockchain_core/Transaction.h"
#include "Blockchain_core/DB/Blockchain_db.h"
#include "Blockchain_core/Token/Token.h"

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


//    std::map<std::string, std::string> map = {{"name", "carrot"}, {"value", "1000"}, {"bytecode", "7b226e616d65223a2022636172726f74222c2022737570706c79223a203130303030307d"}};
//    std::map<std::string, std::string> map = {{"name", "carrot"}, {"value", "1000"}, {"bytecode", "null"}};
//    std::map<std::string, std::string> map = {{"name", "unit"}, {"value", "0"}, {"bytecode", "null"}};
//    Transaction tx = Transaction("g2px1", "", 1,  map, "0", 0);
//    Transaction tx = Transaction("genesis", "g2px1", 0,  map, "0",1000000);
//    Transaction tx = Transaction("g2px1", "gosha", 2,  map, "0", 0);
//    Transaction tx = Transaction("g2px1", "gosha", 0,  map, "0", 10);
//    tx.generate_tx_hash();
//    std::cout << "Transaction: " << tx << std::endl << tx.hash << std::endl;
//    std::cout << tx.to_json_string() << std::endl;
//    Blockchain_db blockchainDb = Blockchain_db();
//    Result<bool> result = blockchainDb.push_transaction(tx);
//    std::cout << result.get_message() << std::endl;

//    rocksdb::Options options;
//    options.create_if_missing = true;
//    options.error_if_exists = false;
//    options.create_missing_column_families = true;
//    options.IncreaseParallelism(cpus);
//    options.OptimizeLevelStyleCompaction();
//    rocksdb::DB* db;
//    Blockchain_db blockchainDb1 = Blockchain_db();
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, blockchainDb1.columnFamilies, &handles, &db);
////    status = db->Delete(rocksdb::WriteOptions(), handles[1], "carrot");
//    std::string value;
//    db->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice("carrot"), &value);
//    std::cout << value << std::endl;
//    db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice("gosha"), &value);
//    std::cout << "Gosha: " << value << std::endl;
//    db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice("g2px1"), &value);
//    std::cout << "Kirill: " << value << std::endl;
    return 0;
}
