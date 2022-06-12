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

//Result<bool> Blockchain_db::push_transaction(Transaction &transaction) {
////    rocksdb::Options options;
////    options.create_if_missing = false;
////    options.error_if_exists = false;
////    rocksdb::DB* db;
////    std::vector<rocksdb::ColumnFamilyHandle*> handles;
////    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
////    status = db->Put(rocksdb::WriteOptions(), columnFamilies[2].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
////    delete db;
////    return Result<bool>(status.ok());
//
//    rocksdb::Options options;
//    rocksdb::TransactionDBOptions txn_db_options;
//    options.create_if_missing = false;
//    options.error_if_exists = false;
//    rocksdb::TransactionDB *txn_db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    rocksdb::WriteOptions write_options;
//    rocksdb::ReadOptions read_options;
//    rocksdb::Status status = rocksdb::TransactionDB::Open(options, txn_db_options, kDBPath, this->columnFamilies, &handles, &txn_db);
//    // pushing tx
//    status = txn_db->Put(rocksdb::WriteOptions(), handles[2], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
//    // begin reading tx
//    rocksdb::Transaction* txn = txn_db->BeginTransaction(write_options);
//    // getting account balance
//    std::string wallet_json;
//    status = txn->Get(read_options, handles[4], rocksdb::Slice(transaction.to), &wallet_json);
//    if (status.IsNotFound()) {
//        // creating account if not found
//        WalletAccount walletAccount;
//        walletAccount.setAddress(transaction.to);
//        if (transaction.type == 0) {
//            // if default transaction - set balance
//            walletAccount.setAmount(transaction.amount);
//        } else {
//            // else set tokens balance
//            std::string token_address;
//            status = txn->Get(read_options, handles[4], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
//            if (status.IsNotFound()) {
//                return Result<bool>(false, "token doesn't exist");
//            }
//            std::map<std::string, double> token_balance = {token_address, transaction.extra_data["value"]};
//            walletAccount.setNonDefaultBalances(token_balance);
//        }
//        txn->Commit();
//        status = txn_db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(walletAccount.to_json_string()));
//        return Result<bool>(true);
//    }
//
//    // if account exists - need to set balance
//    nlohmann::json wallet_js = nlohmann::json::parse(wallet_json);
//    if (transaction.type == 0) {
//        // set units balances
//        wallet_js["amount"] = transaction.amount;
//    } else {
//        std::string token_address;
//        status = txn_db->Get(read_options, handles[4], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
//        if (status.IsNotFound()) {
//            return Result<bool>(false, "token doesn't exist");
//        }
//        bool flag = false;
//        for (nlohmann::json::iterator it = wallet_js["tokens_balance"].begin(); it != wallet_js["tokens_balance"].end(); ++it) {
//            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
//                flag = true;
//                it.value()[transaction.extra_data["name"]] += transaction.extra_data["value"];
//            }
//        }
//
//        if (!flag) { // if user doesn't have balance in current tokens
//            wallet_js["tokens_balance"][wallet_js["tokens_balance"].size()][transaction.extra_data["name"]] = transaction.extra_data["value"];
//        }
//    }
//    status = txn_db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(wallet_js["address"]), rocksdb::Slice(to_string(wallet_js))); // putting
//    status = txn->Commit();
//    delete txn_db; // deleting rocksdb pointer
//    delete txn; // deleting rocksdb pointer
//    return Result<bool>(true);
//}
//
//Result<bool> Blockchain_db::push_transaction_vector(std::vector<Transaction> &transactions) {
//    if(transactions.empty()) return Result<bool>(false, "transaction vector is empty");
//    rocksdb::Options options;
//    options.create_if_missing = false;
//    options.error_if_exists = false;
//    rocksdb::DB* db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
//    for (Transaction transaction : transactions) {
//        status = db->Put(rocksdb::WriteOptions(), columnFamilies[2].name, rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
//    }
//    delete db;
//    return Result<bool>(status.ok());
//}


