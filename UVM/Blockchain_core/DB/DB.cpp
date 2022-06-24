//
// Created by Kirill Zhukov on 21.06.2022.
//

#include "DB.h"

rocksdb::Options unit::DB::get_db_options() {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpuss);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpuss;
    options.delete_obsolete_files_period_micros = 300000000;
    options.env->SetBackgroundThreads(4);
    options.keep_log_file_num = 5;
    return options;
}

std::vector<rocksdb::ColumnFamilyDescriptor> unit::DB::get_column_families() {
    const std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor(ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions())};
    return *&columnFamilies;
}

std::vector<rocksdb::ColumnFamilyHandle*> unit::DB::open_database(rocksdb::DB *db) {
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    return *&handles;
}

std::vector<rocksdb::ColumnFamilyHandle*> unit::DB::open_read_only_database(rocksdb::DB *db) {
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    return *&handles;
}

void unit::DB::create_wallet(std::string &address) {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    WalletAccount walletAccount = WalletAccount(address, 0, {});
    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(address), rocksdb::Slice(walletAccount.to_json_string()));
    close_db(db, &handles);
}

std::optional<std::string> unit::DB::get_block_height() {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    std::string height;
    status = db->Get(rocksdb::ReadOptions(), handles[3], rocksdb::Slice("current"), &height);

    close_db(db, &handles);
    if (height.empty())
        return std::nullopt;
    return height;
}

std::optional<std::string> unit::DB::get_balance(std::string &address) {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    std::string user;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(address), &user);

    close_db(db, &handles);
    if (user.empty())
        return std::nullopt;
    return user;
}

std::optional<std::string> unit::DB::create_new_token(Transaction *transaction) {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
    nlohmann::json transaction_parser = nlohmann::json::parse(transaction->to_json_string());

    if (!transaction_parser["extradata"].contains("bytecode")) {
        close_db(db, &handles);
        return std::nullopt;
    }

    nlohmann::json parsed_bytecode;
    try {
        std::string hex = transaction_parser["extradata"]["bytecode"];
        unit::DB::normalize_str(&hex);
        parsed_bytecode = nlohmann::json::parse(hex_to_ascii(hex));
    } catch (std::exception &e) {
        close_db(db, &handles);
        return std::nullopt;
    }

    std::string token_name = parsed_bytecode["name"];
    unit::DB::normalize_str(&token_name);
    std::string token_from_db;
    status = db->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(token_name), &token_from_db);

    if (!token_from_db.empty()) {
        close_db(db, &handles);
        delete transaction;
        return std::nullopt;
    }

    double supply = parsed_bytecode["supply"];
    Token token = Token(token_name, transaction->extra_data["bytecode"], transaction->from, supply);
    status = db->Put(rocksdb::WriteOptions(), handles[1], rocksdb::Slice(token.name), rocksdb::Slice(token.to_json_string()));
    transaction->setTo(token.token_hash);

    if (!status.ok()) {
        close_db(db, &handles);
        return std::nullopt;
    }

    // now we need to increase creator's wallet with current tokens supply
    std::string from;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction->from), &from);
    if (!status.ok()) {
        close_db(db, &handles);
        return std::nullopt;
    }
    nlohmann::json creator = nlohmann::json::parse(from);
    creator["tokens_balance"][creator["tokens_balance"].size()][token_name] = supply;
    std::string prepared_wallet = creator.dump();
    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction->from), rocksdb::Slice(prepared_wallet));

    close_db(db, &handles);
    if (!status.ok())
        return std::nullopt;

    return token.token_hash;
}

bool unit::DB::push_transaction(Transaction *transaction) {
    std::optional<std::string> op_recipient = unit::DB::get_balance(transaction->to);
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    if (!op_recipient.has_value())
        unit::DB::create_wallet(transaction->to);

    if (transaction->type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction->type == CREATE_TOKEN)
        goto create_token;
    else
        goto transfer_tokens;


    unit_transfer: {
        op_recipient = unit::DB::get_balance(transaction->to);
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_recipient.value());
        parsed_wallet["amount"] = parsed_wallet["amount"].get<double>() + transaction->amount;
        status = db->Put(rocksdb::WriteOptions(), handles[2], rocksdb::Slice(transaction->hash), rocksdb::Slice(transaction->to_json_string()));
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction->to), rocksdb::Slice(parsed_wallet.dump()));

        close_db(db, &handles);
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
            close_db(db, &handles);
            delete transaction;
            return false;
        }

        std::optional op_token = unit::DB::create_new_token(transaction);
        if (!op_token.has_value()) {
            close_db(db, &handles);
            delete transaction;
            return false;
        }
        status = db->Put(rocksdb::WriteOptions(), handles[2], rocksdb::Slice(transaction->hash), rocksdb::Slice(transaction->to_json_string()));

        close_db(db, &handles);
        if (!status.ok())
            return false;
        transaction->generate_tx_hash();
        return true;
    };

    transfer_tokens: {
        std::optional op_token = unit::DB::get_token(transaction->extra_data["name"]);
        if (!op_token.has_value()) {
            close_db(db, &handles);
            return false;
        }

        op_recipient = unit::DB::get_balance(transaction->to);
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
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction->to), rocksdb::Slice(parsed_wallet.dump()));

        close_db(db, &handles);
        if (!status.ok())
            return false;
        return true;
    };
}

std::optional<std::string> unit::DB::get_token(std::string &token_address) {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    std::string token;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(token_address), &token);

    close_db(db, &handles);
    if (token.empty())
        return std::nullopt;
    return token;
}

bool unit::DB::validate_sender_balance(Transaction &transaction) {
    std::optional<std::string> op_sender = unit::DB::get_balance(transaction.from);
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    if (!op_sender.has_value()) {
        close_db(db, &handles);
        return false;
    }

    if (transaction.type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction.type == CREATE_TOKEN) {
        close_db(db, &handles);
        return true;
    } else
        goto transfer_tokens;

    unit_transfer: {
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
        parsed_wallet["amount"] = parsed_wallet["amount"].get<double>() - transaction.amount;
        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction.from),rocksdb::Slice(parsed_wallet.dump()));

        close_db(db, &handles);
        if (!status.ok())
            return false;
        return true;
    };

    transfer_tokens: {
        std::optional op_token = unit::DB::get_token(transaction.extra_data["name"]);
        nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
        bool balance_in_token = false;
        for (nlohmann::json::iterator it = parsed_wallet["tokens_balance"].begin(); it != parsed_wallet["tokens_balance"].end(); ++it) {
            if (it.value().contains(transaction.extra_data["name"])) { // if user already has balance in current tokens
                it.value()[transaction.extra_data["name"]] = it.value()[transaction.extra_data["name"]].get<double>() - std::stod(transaction.extra_data["value"]);
                balance_in_token = true;
            }
        }

        if (!balance_in_token) {
            close_db(db, &handles);
            return false;
        }

        status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction.from),rocksdb::Slice(parsed_wallet.dump()));

        close_db(db, &handles);
        if (!status.ok())
            return false;
        return true;
    };
}

bool unit::DB::push_block(Block block) {
    std::optional<std::string> op_current = unit::DB::get_block_height();
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    if (op_current.has_value())
        goto common;
    else
        goto genesis;

    genesis: {
        block.setIndex(1);
        block.setPrevHash("genesis");
        goto push_values;
    };

    common: {
        nlohmann::json parsed_current = nlohmann::json::parse(op_current.value());
        std::string prev_hash = parsed_current["hash"].get<std::string>();
        block.setPrevHash(prev_hash);

        if (block.getIndex() % 100 == 0) {
            rocksdb::Slice begin("genesis");
            rocksdb::Slice end(block.getHash());
            rocksdb::CompactRangeOptions options;
            status = db->CompactRange(options, &begin, &end);
            status = db->FlushWAL(true);
        }
        goto push_values;
    };

    push_values: {
        std::cout << "block #" << block.getIndex() << ": " << block.to_json_with_tx_hash_only() << std::endl;
        status = db->Put(rocksdb::WriteOptions(), handles[0], rocksdb::Slice(block.hash), rocksdb::Slice(block.to_json_with_tx_hash_only()));
        if (!status.ok()) {
            close_db(db, &handles);
            return false;
        }

        status = db->Put(rocksdb::WriteOptions(), handles[3], rocksdb::Slice("current"), rocksdb::Slice(block.to_json_with_tx_hash_only()));

        close_db(db, &handles);
        if (!status.ok()) {
            return false;
        }

        return true;
    };
}

void unit::DB::close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    for (auto handle : *handles)
        db->DestroyColumnFamilyHandle(handle);
    delete db;
}