//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "Blockchain_db.h"

Result<bool> Blockchain_db::push_block(Block block,  Result<bool> &res) {
//    Result<bool> result = this->get_block_height();
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 1;
    options.keep_log_file_num = 5;
    options.recycle_log_file_num = true;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
    if (res.getSupportingResult().empty()) {
        block.setIndex(1);
        block.setPrevHash("0");
    } else {
        nlohmann::json block_json = nlohmann::json::parse(res.getSupportingResult());
        uint64_t index = block_json["index"];
        block.setIndex(index + 1);
        std::string previous_hash = to_string(block_json["hash"]);
        previous_hash.erase(
                std::remove(previous_hash.begin(), previous_hash.end(), '\"'),
                previous_hash.end());
        block.setPrevHash(previous_hash);
    }
    block.generate_hash();
    if (block.getIndex() % 100 == 0) {
        rocksdb::Slice begin("0");
        rocksdb::Slice end(block.getHash());
        rocksdb::CompactRangeOptions options;
        status = db->CompactRange(options, &begin, &end);
        status = db->FlushWAL(true);
    }
    std::cout << "block #" << block.getIndex() << ": " << block.to_json_with_tx_hash_only() << std::endl;
    status = db->Put(rocksdb::WriteOptions(), handles[0], rocksdb::Slice(block.hash), rocksdb::Slice(block.to_json_with_tx_hash_only()));
    status = db->Put(rocksdb::WriteOptions(), handles[3], rocksdb::Slice("current"), rocksdb::Slice(block.to_json_with_tx_hash_only()));
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
    }
    delete db;
    db = nullptr;
    return Result<bool>(status.ok());
}

Result<bool> Blockchain_db::push_transaction(Transaction transaction) {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.compression = rocksdb::kSnappyCompression;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 300000000;
    options.keep_log_file_num = 5;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &db);

    std::cout << transaction << std::endl;
//     getting account balance
    std::string wallet_json;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.to), &wallet_json);
    if (status.IsNotFound()) {
//        std::cout << "not found" << std::endl;
        // creating account if not found
        WalletAccount walletAccount;
        walletAccount.setAddress(transaction.to);
        if (transaction.type == UNIT_TRANSFER) {
            // if default transaction - set balance
            walletAccount.setAmount(transaction.amount);
        } else if (transaction.type == CREATE_TOKEN) {
            nlohmann::json parsed_tx = nlohmann::json::parse(transaction.to_json_string());
            std::string bytecode = parsed_tx["extradata"]["bytecode"];
            nlohmann::json token_info = nlohmann::json::parse(hex_to_ascii(bytecode));
            std::string token_name = nlohmann::to_string(token_info["name"]);
            token_name.erase(
                    std::remove(token_name.begin(), token_name.end(), '\"'),
                    token_name.end());
            delete db;
            Result<bool> result = this->create_new_token(token_name, token_info["supply"], transaction.from, bytecode, transaction);
            if (!result.ok()) {
                for (auto handle : handles) {
                    status = db->DestroyColumnFamilyHandle(handle);
                }
                delete db; // deleting rocksdb pointer
                return Result<bool>(false, result.get_message());
            }
            transaction.setTo(result.getSupportingResult());
            status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &db);
        } else {
            // else set tokens balance
            std::string token_address;
            status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
            if (!status.ok()) {
                for (auto handle : handles) {
                    status = db->DestroyColumnFamilyHandle(handle);
                }
                delete db; // deleting rocksdb pointer
                return Result<bool>(false, "unexpected error");
            }
            std::map<std::string, std::string> token_balance = {{token_address, transaction.extra_data["value"]}};
            walletAccount.setNonDefaultBalances(token_balance);
        }
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(walletAccount.to_json_string()));
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return Result<bool>(true, "null", transaction.hash);
    }

    // if account exists - need to set balance
    nlohmann::json wallet_js = nlohmann::json::parse(wallet_json);
    if (transaction.type == UNIT_TRANSFER) {
        // set units balances
        std::string balance = to_string(wallet_js["amount"]);
        double d_balance = std::stod(balance) + transaction.amount;
        wallet_js["amount"] = d_balance;
    } else if (transaction.type == CREATE_TOKEN) {
        nlohmann::json parsed_tx = nlohmann::json::parse(transaction.to_json_string());
        std::string bytecode = parsed_tx["extradata"]["bytecode"];
        nlohmann::json token_info = nlohmann::json::parse(hex_to_ascii(bytecode));
        std::string token_name = nlohmann::to_string(token_info["name"]);
        token_name.erase(
                std::remove(token_name.begin(), token_name.end(), '\"'),
                token_name.end());
        delete db;
        Result<bool> result = this->create_new_token(token_name, token_info["supply"], transaction.from, bytecode, transaction);
        if (!result.ok()) {
            for (auto handle : handles) {
                status = db->DestroyColumnFamilyHandle(handle);
            }
            delete db; // deleting rocksdb pointer
            return Result<bool>(false, result.get_message());
        }
        transaction.setTo(result.getSupportingResult());
        status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &db);
    } else {
        std::string token_address;
        status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.extra_data["name"]), &token_address);
        if (!token_address.empty()) {
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
    transaction.generate_tx_hash();
    if (transaction.type > 1) {
        std::string token;
        db->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(transaction.extra_data["name"]), &token);
        nlohmann::json token_json = nlohmann::json::parse(token);
        token_json["transactions"].push_back(transaction.hash);
        status = db->Put(rocksdb::WriteOptions(), handles[1], rocksdb::Slice(transaction.extra_data["name"]), rocksdb::Slice(to_string(token_json))); // putting
    }
    // pushing tx
    status = db->Put(rocksdb::WriteOptions(), handles[2], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
    if (!status.ok()) {
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db; // deleting rocksdb pointer
        return Result<bool>(false, "unexpected error");
    }
    std::string address = wallet_js["address"];
    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(address), rocksdb::Slice(to_string(wallet_js))); // putting
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
    }
    delete db; // deleting rocksdb pointer
    return Result<bool>(true, "null", transaction.hash);
}

Result<bool> Blockchain_db::create_new_token(std::string &name, double supply, std::string &creator, std::string bytecode, Transaction &transaction) {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 300000000;
    options.keep_log_file_num = 5;
    rocksdb::DB* token_db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &token_db);
    std::string token_from_db;
    status = token_db->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(name), &token_from_db);
    if (!token_from_db.empty()) {
        for (auto handle : handles) {
            status = token_db->DestroyColumnFamilyHandle(handle);
        }
        delete token_db;
        return Result(false, "already exist"); // check if token already exists
    }
    std::vector<std::string> txs = {transaction.hash};
    Token token = Token(name, bytecode, transaction.from, supply);
    token.generate_hash();
//    token.setTransactionsInToken(txs);
    status = token_db->Put(rocksdb::WriteOptions(), handles[1], rocksdb::Slice(token.name), rocksdb::Slice(token.to_json_string())); // putting
    if (!status.ok()) {
//        std::cout << "token put" << std::endl;
        for (auto handle : handles) {
            status = token_db->DestroyColumnFamilyHandle(handle);
        }
        delete token_db;
        return Result(false, "unexpected error");
    }
    std::string wallet_json;
    status = token_db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(creator), &wallet_json);
//    std::cout << wallet_json << std::endl;
    nlohmann::json sender_wallet_js = nlohmann::json::parse(wallet_json);
    sender_wallet_js["tokens_balance"][sender_wallet_js["tokens_balance"].size()][name] = supply;
//    std::cout << sender_wallet_js << std::endl;
    if (!status.ok()) {
//        std::cout << "wallet js" << std::endl;
        for (auto handle : handles) {
            status = token_db->DestroyColumnFamilyHandle(handle);
        }
        delete token_db;
        return Result(false, "unexpected error");
    }

    status = token_db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(creator), rocksdb::Slice(to_string(sender_wallet_js)));
    if (!status.ok()) {
        for (auto handle : handles) {
            status = token_db->DestroyColumnFamilyHandle(handle);
        }
        delete token_db;
        return Result(false, "unexpected error");
    }
    return Result(true, "null", token.token_hash);
}

Result<bool> Blockchain_db::get_block_height() {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.compression = rocksdb::kSnappyCompression;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 1;
    options.keep_log_file_num = 5;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, kDBPath, this->columnFamilies, &handles, &db);
    std::string height;
    status = db->Get(rocksdb::ReadOptions(), handles[3], rocksdb::Slice("current"), &height);
    if (height.empty()) {
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
            handle = nullptr;
        }
        delete db;
        db = nullptr;
        return Result<bool>(true, "block not found", "");
    }
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
        handle = nullptr;
    }
    delete db;
    db = nullptr;
    return Result<bool>(true, "null", height);
}

Result<bool> Blockchain_db::get_balance(std::string &address) {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.keep_log_file_num = 5;
    options.max_open_files = 10;
    options.max_background_jobs = cpus/2;
    options.max_subcompactions = cpus/2;
    options.delete_obsolete_files_period_micros = 300000000;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, kDBPath, this->columnFamilies, &handles, &db);

    std::string user;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(address), &user);
    if (user.empty()) {
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return Result<bool>(true, "user not found", "null");
    }
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
    }
    delete db;

    return Result<bool>(true, "null", user);
}

bool Blockchain_db::validate_sender_balance(Transaction &transaction) {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 300000000;
    options.keep_log_file_num = 5;
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, this->columnFamilies, &handles, &db);
    // sender's balance
    std::string sender_balance;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.from), &sender_balance);
    if (sender_balance.empty()) {
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return false;
    }
    nlohmann::json sender_wallet_js = nlohmann::json::parse(sender_balance);
    if (transaction.type == UNIT_TRANSFER) {
        // sender's balance
        double balance_amount = sender_wallet_js["amount"];
        sender_wallet_js["amount"] = balance_amount - transaction.amount; // subtracting value
        if (sender_wallet_js["amount"] < 0) {
            for (auto handle : handles) {
                status = db->DestroyColumnFamilyHandle(handle);
            }
            delete db;
            return false;
        }
        std::string sender_addr = sender_wallet_js["address"]; // getting sender's address
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js)));
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return true;
    } else if (transaction.type == TOKEN_TRANSFER) {
        // sender's balance
        for (nlohmann::json::iterator it = sender_wallet_js["tokens_balance"].begin(); it != sender_wallet_js["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                double balance = it.value()[transaction.extra_data["name"]];
                balance = balance - std::stod(transaction.extra_data["value"]);
                it.value()[transaction.extra_data["name"]] = balance;
            }
        }
        std::string sender_addr = sender_wallet_js["address"];
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(sender_addr), rocksdb::Slice(to_string(sender_wallet_js))); // putting
        //end manipulating with sender's balance
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return true;
    } else {
        for (auto handle : handles) {
            status = db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
        return true;
    }
}
