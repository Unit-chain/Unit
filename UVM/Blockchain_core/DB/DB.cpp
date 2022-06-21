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

void unit::DB::create_wallet(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::string &address) {
    WalletAccount walletAccount = WalletAccount(address, 0, {});
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(address), rocksdb::Slice(walletAccount.to_json_string()));
}

std::optional<std::string> unit::DB::get_block_height(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    std::string height;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[3], rocksdb::Slice("current"), &height);
    if (height.empty())
        return std::nullopt;
    return height;
}

std::optional<std::string> unit::DB::get_balance(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::string &address) {
    std::string user;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[4], rocksdb::Slice(address), &user);
    if (user.empty())
        return std::nullopt;
    return user;
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
    transaction->setTo(token.token_hash);

    if (!status.ok())
        return std::nullopt;

    // now we need to increase creator's wallet with current tokens supply
    std::string from;
    status = db->Get(rocksdb::ReadOptions(), (*handles)[4], rocksdb::Slice(transaction->from), &from);
    if (!status.ok())
        return std::nullopt;
    nlohmann::json creator = nlohmann::json::parse(from);
    creator["tokens_balance"][creator["tokens_balance"].size()][token_name] = supply;
    std::string prepared_wallet = creator.dump();
    status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(transaction->from), rocksdb::Slice(prepared_wallet));
    if (!status.ok())
        return std::nullopt;

    return token.token_hash;
}

bool unit::DB::push_transaction(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, Transaction *transaction) {
    std::optional<std::string> op_recipient = unit::DB::get_balance(db, handles, transaction->to);

    if (!op_recipient.has_value())
        unit::DB::create_wallet(db, handles, transaction->to);

    if (transaction->type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction->type == CREATE_TOKEN)
        goto create_token;
    else
        goto transfer_tokens;

    unit_transfer: {
        op_recipient = unit::DB::get_balance(db, handles, transaction->to);
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_recipient.value());
        parsed_wallet["amount"] = parsed_wallet["amount"].get<double>() + transaction->amount;
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[2], rocksdb::Slice(transaction->hash), rocksdb::Slice(transaction->to_json_string()));
        status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(transaction->to), rocksdb::Slice(parsed_wallet.dump()));
        if (!status.ok())
            return false;

        transaction->generate_tx_hash();
        return true;
    };

    create_token: {
        nlohmann::json parsed_tx = nlohmann::json::parse(transaction->to_json_string());
        std::string bytecode = parsed_tx["extradata"]["bytecode"].dump();
        try {
            nlohmann::json bytecode_parser = nlohmann::json::parse(hex_to_ascii(bytecode));
        } catch (std::exception &e) {
            delete transaction;
            return false;
        }

        std::optional op_token = unit::DB::create_new_token(db, handles, transaction);
        if (!op_token.has_value()) {
            delete transaction;
            return false;
        }
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[2], rocksdb::Slice(transaction->hash), rocksdb::Slice(transaction->to_json_string()));
        if (!status.ok())
            return false;

        transaction->generate_tx_hash();
        return true;
    };

    transfer_tokens: {
        std::optional op_token = unit::DB::get_token(db, handles, transaction->extra_data["name"]);
        if (!op_token.has_value())
            return false;
        op_recipient = unit::DB::get_balance(db, handles, transaction->to);
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_recipient.value());
        bool balance_in_token = false;
        for (nlohmann::json::iterator it = parsed_wallet["tokens_balance"].begin(); it != parsed_wallet["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction->extra_data["name"])) { // if user already has balance in current tokens
                it.value()[transaction->extra_data["name"]] = it.value()[transaction->extra_data["name"]].get<double>() + std::stod(transaction->extra_data["value"]);
                balance_in_token = true;
            }
        }

        if (!balance_in_token)
            parsed_wallet["tokens_balance"][parsed_wallet["tokens_balance"].size()][transaction->extra_data["name"]] = transaction->extra_data["value"];
        transaction->generate_tx_hash();

        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(transaction->to), rocksdb::Slice(parsed_wallet.dump()));

        if (!status.ok())
            return false;
        return true;
    };
}

std::optional<std::string> unit::DB::get_token(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::string &token_address) {
    std::string token;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), (*handles)[4], rocksdb::Slice(token_address), &token);
    if (token.empty())
        return std::nullopt;
    return token;
}

bool unit::DB::validate_sender_balance(rocksdb::DB *db, std::vector<rocksdb::ColumnFamilyHandle *> *handles, Transaction &transaction) {
    std::optional<std::string> op_sender = unit::DB::get_balance(db, handles, transaction.from);
    if (!op_sender.has_value())
        return false;

    if (transaction.type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction.type == CREATE_TOKEN)
        return true;
    else
        goto transfer_tokens;

    unit_transfer: {
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
        parsed_wallet["amount"] = parsed_wallet["amount"].get<double>() - transaction.amount;
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(transaction.from),rocksdb::Slice(parsed_wallet.dump()));
        if (!status.ok())
            return false;
        return true;
    };

    transfer_tokens: {
        std::optional op_token = unit::DB::get_token(db, handles, transaction.extra_data["name"]);
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
        bool balance_in_token = false;
        for (nlohmann::json::iterator it = parsed_wallet["tokens_balance"].begin(); it != parsed_wallet["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                it.value()[transaction.extra_data["name"]] = it.value()[transaction.extra_data["name"]].get<double>() - std::stod(transaction.extra_data["value"]);
                balance_in_token = true;
            }
        }

        if (!balance_in_token)
            return false;

        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), (*handles)[4], rocksdb::Slice(transaction.from),rocksdb::Slice(parsed_wallet.dump()));

        if (!status.ok())
            return false;

        return true;
    };
}

void unit::DB::close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    for (auto handle : (*handles))
        db->DestroyColumnFamilyHandle(handle);
    delete db;
}

