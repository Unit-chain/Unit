//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "Blockchain_db.h"

void Blockchain_db::start_node_db() {
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

Result<bool> Blockchain_db::push_block(Block &block) {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
    status = db->Put(rocksdb::WriteOptions(), columnFamilies[0].name, rocksdb::Slice(block.hash), rocksdb::Slice(block.to_json_with_tx_hash_only()));
    delete db;
    return Result<bool>(status.ok());
}

Result<bool> Blockchain_db::push_transaction(Transaction &transaction) {
//    rocksdb::Options options;
//    options.create_if_missing = false;
//    options.error_if_exists = false;
//    rocksdb::DB* db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
//    status = db->Put(rocksdb::WriteOptions(), columnFamilies[2].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
//    delete db;
//    return Result<bool>(status.ok());

    rocksdb::Options options;
    rocksdb::TransactionDBOptions txn_db_options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    rocksdb::TransactionDB* txn_db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::WriteOptions write_options;
    rocksdb::Status status = rocksdb::TransactionDB::Open(options, txn_db_options, kDBPath, this->columnFamilies, &handles, &txn_db);
    rocksdb::Transaction* txn = txn_db->BeginTransaction(write_options);
    status = txn_db->Put(rocksdb::WriteOptions(), columnFamilies[2].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
    status = txn_db->Put(rocksdb::WriteOptions(), columnFamilies[4].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
}

Result<bool> Blockchain_db::push_transaction_vector(std::vector<Transaction> &transactions) {
    if(transactions.empty()) return Result<bool>(false, "transaction vector is empty");
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
    for (Transaction transaction : transactions) {
        status = db->Put(rocksdb::WriteOptions(), columnFamilies[2].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
    }
    delete db;
    return Result<bool>(status.ok());
}


