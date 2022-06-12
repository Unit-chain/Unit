#include <assert.h>
#include <string.h>
#include "rocksdb/db.h"
#include <iostream>
#include <nlohmann/json.hpp>

#include <thread>
#include "vector"
#include "Blockchain_core/Wallet/WalletAccount.h"

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

    WalletAccount walletAccount;
    walletAccount.setAmount(1000);
    walletAccount.setAddress("g2px1");
    std::map<std::string, double> tokens = {{"carrot", 10}, {"meat", 100}};
    walletAccount.setNonDefaultBalances(tokens);
    std::string wall = walletAccount.to_json_string();
    std::cout << wall << std::endl;
    nlohmann::json js = nlohmann::json::parse(wall);
    std::cout << js["tokens_balance"] << std::endl;
    js["tokens_balance"][2]["beer"] = 1000;
    std::cout << js << std::endl << js["tokens_balance"].size();

    return 0;
}
