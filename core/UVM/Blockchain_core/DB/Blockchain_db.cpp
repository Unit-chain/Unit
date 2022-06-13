//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "Blockchain_db.h"
/*
    it'll be deprecate in future versions
*/
//Result<bool> Blockchain_db::start_node_db() {
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    options.error_if_exists = false;
//    options.create_missing_column_families = true;
//    options.IncreaseParallelism(cpus);
//    options.OptimizeLevelStyleCompaction();
//    rocksdb::DB* db;
//    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
//
//    for (const auto & columnFamiliesName : columnFamiliesNames) { // creating column families
//        rocksdb::ColumnFamilyHandle* cf;
//        std::cout << "Creating column families: " << columnFamiliesName << std::endl;
//        status = db->CreateColumnFamily(rocksdb::ColumnFamilyOptions(), columnFamiliesName, &cf);
//        db->DestroyColumnFamilyHandle(cf); // delete ptr
//    }
//    delete db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
//    for (auto handle : handles) {
//        status = db->DestroyColumnFamilyHandle(handle);
//        std::cout << "Destructing column families handler status: " << status.ok() << std::endl;
//    }
//    delete db;
//    return (status.ok()) ? Result<bool>(true) : Result<bool>(status.ok(), "column families already exists");
//}

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
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.compression = rocksdb::kSnappyCompression;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &db);

    // pushing tx
    status = db->Put(rocksdb::WriteOptions(), handles[3], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
    if (!status.ok()) return Result<bool>(false, "unexpected error");

    // sender's balance
    std::string sender_balance;
    status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.from), &sender_balance);
    nlohmann::json sender_wallet_js = nlohmann::json::parse(sender_balance);
    // getting account balance
    std::string wallet_json;
    status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.to), &wallet_json);
    if (status.IsNotFound()) {
        std::cout << "not found" << std::endl;
        // creating account if not found
        WalletAccount walletAccount;
        walletAccount.setAddress(transaction.to);
        if (transaction.type == 0) {
            // if default transaction - set balance
            walletAccount.setAmount(transaction.amount);
            // sender's balance
            double balance_amount = sender_wallet_js["amount"];
            sender_wallet_js["amount"] = balance_amount - transaction.amount;
            std::string sender_addr = sender_wallet_js["address"];
            status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js)));
        } else {
            // else set tokens balance
            std::string token_address;
            status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
            if (status.IsNotFound()) {
                return Result<bool>(false, "token doesn't exist");
            }
            if (status.IsNotFound()) return Result<bool>(false, "token doesn't exist");
            std::map<std::string, std::string> token_balance = {{token_address, transaction.extra_data["value"]}};
            walletAccount.setNonDefaultBalances(token_balance);
            for (nlohmann::json::iterator it = sender_wallet_js["tokens_balance"].begin(); it != sender_wallet_js["tokens_balance"].end(); ++it) {
                if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                    std::string balance = it.value()[transaction.extra_data["name"]];
                    balance = std::to_string(std::stod(balance) - std::stod(transaction.extra_data["value"]));
                    it.value()[transaction.extra_data["name"]] = balance;
                }
            }
            std::string sender_addr = sender_wallet_js["address"];
            status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js))); // putting
            //end manipulating with sender's balance
        }
        status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(transaction.to), rocksdb::Slice(walletAccount.to_json_string()));
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        return Result<bool>(true);
    }

    // if account exists - need to set balance
    nlohmann::json wallet_js = nlohmann::json::parse(wallet_json);
    if (transaction.type == 0) {
        // set units balances
        std::string balance = to_string(wallet_js["amount"]);
        double d_balance = std::stod(balance) + transaction.amount;
        wallet_js["amount"] = d_balance;
        // sender's balance
        double balance_amount = sender_wallet_js["amount"];
        sender_wallet_js["amount"] = balance_amount - transaction.amount; // subtracting value
        std::string sender_addr = sender_wallet_js["address"]; // getting sender's address
        status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js)));
    } else {
        std::string token_address;
        status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
//        if (status.IsNotFound()) {
//            return Result<bool>(false, "token doesn't exist");
//        }
        bool flag = false;
        for (nlohmann::json::iterator it = wallet_js["tokens_balance"].begin(); it != wallet_js["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                flag = true;
                std::string balance = it.value()[transaction.extra_data["name"]];
                balance = std::to_string(std::stod(transaction.extra_data["value"]) + std::stod(balance));
                it.value()[transaction.extra_data["name"]] = balance;
            }
        }


        // sender's balance
        for (nlohmann::json::iterator it = sender_wallet_js["tokens_balance"].begin(); it != sender_wallet_js["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                std::string balance = it.value()[transaction.extra_data["name"]];
                balance = std::to_string(std::stod(balance) - std::stod(transaction.extra_data["value"]));
                it.value()[transaction.extra_data["name"]] = balance;
            }
        }
        std::string sender_addr = sender_wallet_js["address"];
        status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js))); // putting
        //end manipulating with sender's balance


        if (!flag) { // if user doesn't have balance in current tokens
            wallet_js["tokens_balance"][wallet_js["tokens_balance"].size()][transaction.extra_data["name"]] = transaction.extra_data["value"];
        }
    }
    std::string address = wallet_js["address"];
    status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(address), rocksdb::Slice(to_string(wallet_js))); // putting
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
    }
    delete db; // deleting rocksdb pointer
    return Result<bool>(true);
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
        // pushing tx
        status = db->Put(rocksdb::WriteOptions(), handles[3], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
        if (!status.ok()) return Result<bool>(false, "unexpected error");

        // getting account balance
        std::string wallet_json;
        status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.to), &wallet_json);
        if (status.IsNotFound()) {
            std::cout << "not found" << std::endl;
            // creating account if not found
            WalletAccount walletAccount;
            walletAccount.setAddress(transaction.to);
            if (transaction.type == 0) {
                // if default transaction - set balance
                walletAccount.setAmount(transaction.amount);
            } else {
                // else set tokens balance
                std::string token_address;
                status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
                if (status.IsNotFound()) return Result<bool>(false, "token doesn't exist");
                std::map<std::string, std::string> token_balance = {{token_address, transaction.extra_data["value"]}};
                walletAccount.setNonDefaultBalances(token_balance);
            }
            status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(transaction.to), rocksdb::Slice(walletAccount.to_json_string()));
            for (auto handle : handles) {
                status = db->DestroyColumnFamilyHandle(handle);
            }
            return Result<bool>(true);
        }

        // if account exists - need to set balance
        nlohmann::json wallet_js = nlohmann::json::parse(wallet_json);
        if (transaction.type == 0) {
            // set units balances
//        std::cout <<  wallet_js["amount"] << std::endl;
            std::string balance = to_string(wallet_js["amount"]);
            double d_balance = std::stod(balance) + transaction.amount;
            wallet_js["amount"] = d_balance;
        } else {
            std::string token_address;
            status = db->Get(rocksdb::ReadOptions(), handles[5], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
            if (status.IsNotFound()) {
                return Result<bool>(false, "token doesn't exist");
            }
            bool flag = false;
            for (nlohmann::json::iterator it = wallet_js["tokens_balance"].begin(); it != wallet_js["tokens_balance"].end(); ++it) {
                if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                    flag = true;
                    std::string balance = it.value()[transaction.extra_data["name"]];
                    balance = std::to_string(std::stod(transaction.extra_data["value"]) + std::stod(balance));
                    it.value()[transaction.extra_data["name"]] = balance;
                }
            }

            if (!flag) { // if user doesn't have balance in current tokens
                wallet_js["tokens_balance"][wallet_js["tokens_balance"].size()][transaction.extra_data["name"]] = transaction.extra_data["value"];
            }
        }
        std::string address = wallet_js["address"];
        status = db->Put(rocksdb::WriteOptions(), handles[5], rocksdb::Slice(address), rocksdb::Slice(to_string(wallet_js))); // putting
    }
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
    }
    delete db; // deleting rocksdb pointer
    return Result<bool>(true);
}


