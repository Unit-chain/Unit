//
// Created by Kirill Zhukov on 21.06.2022.
//

#include "DB.h"

rocksdb::Options unit::DB::get_db_options() {
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
    return options;
}

std::vector<rocksdb::ColumnFamilyHandle*> unit::DB::open_database(rocksdb::DB *db) {
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kDBPath, this->columnFamilies, &handles, &db);
    return handles;
}

std::vector<rocksdb::ColumnFamilyHandle*> unit::DB::open_read_only_database(rocksdb::DB *db) {
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kDBPath, this->columnFamilies, &handles, &db);

    return handles;
}

void unit::DB::create_wallet(std::string &address, rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    WalletAccount walletAccount = WalletAccount(address, 0, {});
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(address), rocksdb::Slice(walletAccount.to_json_string()));
}

std::optional<std::string> unit::DB::get_block_height(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    std::string height;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[3], rocksdb::Slice("current"), &height);

    if (height.empty()) {
        return std::nullopt;
    }

    return height;
}

std::optional<std::string> unit::DB::get_balance(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::string &address) {
    std::string user;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[4], rocksdb::Slice(address), &user);

    if (user.empty()) {
        return std::nullopt;
    }

    return user;
}

void unit::DB::close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    for (auto handle : (*handles)) {
        db->DestroyColumnFamilyHandle(handle);
    }
    delete db;
}

std::optional<std::string> unit::DB::create_new_token(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, Transaction *transaction) {
    nlohmann::json transaction_parser = nlohmann::json::parse(transaction->to_json_string());

    if (!transaction_parser["extradata"].contains("bytecode"))
        return std::nullopt;

    nlohmann::json parsed_bytecode;
    try {
        std::string hex = transaction_parser["extradata"]["bytecode"];
        unit::DB::normalize_str(&hex);
        parsed_bytecode = nlohmann::json::parse(hex_to_ascii(hex));
    } catch (std::exception &e) {
        return std::nullopt;
    }

    std::string token_name = parsed_bytecode["name"];
    unit::DB::normalize_str(&token_name);
    std::string token_from_db;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[1], rocksdb::Slice(token_name), &token_from_db);

    if (!token_from_db.empty()) {
        delete transaction;
        return std::nullopt;
    }

    double supply = parsed_bytecode["supply"];
    Token token = Token(token_name, transaction->extra_data["bytecode"], transaction->from, supply);
    status = db->Put(rocksdb::WriteOptions(), (*handles)[1], rocksdb::Slice(token.name), rocksdb::Slice(token.to_json_string()));

    if (!status.ok())
        return std::nullopt;

    // now we need to increase creator wallet with current tokens supply



    return std::optional<std::string>();
}


