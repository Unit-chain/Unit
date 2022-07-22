//
// Created by Kirill Zhukov on 21.06.2022.
//

#include "DB.h"
#include "boost/json/src.hpp"
#include "boost/json/array.hpp"
#include "boost/json/object.hpp"

rocksdb::Options unit::DB::get_db_options() {
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpuss);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kZSTD;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpuss;
    options.env->SetBackgroundThreads(cpuss);
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

    if(!status.ok()) {
        close_db(db, &handles);
        return std::nullopt;
    }

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

//    if(!unit::DB::validate_sender_balance(transaction))
//        return false;

    if (!op_recipient.has_value())
        unit::DB::create_wallet(transaction->to);

    if (transaction->type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction->type == CREATE_TOKEN)
        goto create_token;
    else if (transaction->type == TOKEN_TRANSFER)
        goto transfer_tokens;
    else
        return false;


    unit_transfer: {
        rocksdb::DB *db;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

        nlohmann::json parsed_wallet = nlohmann::json::parse(op_recipient.value());

    //        if(parsed_wallet["amount"].get<double>() < transaction->amount)
    //            return false;

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
        rocksdb::DB *db;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

        status = db->Put(rocksdb::WriteOptions(), handles[2], rocksdb::Slice(transaction->hash), rocksdb::Slice(transaction->to_json_string()));

        close_db(db, &handles);
        if (!status.ok())
            return false;
        transaction->generate_tx_hash();
        return true;
    };

    transfer_tokens: {
        rocksdb::DB *db;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

        std::optional op_token = unit::DB::get_token(transaction->extra_data["name"]);
        if (!op_token.has_value()) {
            close_db(db, &handles);
            return false;
        }
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

bool unit::DB::push_transactions(Block *block) {
    rocksdb::Options options;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::OptimisticTransactionDBOptions optimisticTransactionDbOptions;
    rocksdb::OptimisticTransactionDB* txn_db;
    rocksdb::Status s = rocksdb::OptimisticTransactionDB::Open(rocksdb::DBOptions(), optimisticTransactionDbOptions, kkDBPath, get_column_families(), &handles, &txn_db);
    rocksdb::WriteOptions write_options;
    rocksdb::ReadOptions read_options;
    rocksdb::OptimisticTransactionOptions txn_options;
    std::string value;
    txn_options.set_snapshot = true;
    rocksdb::Transaction* txn = txn_db->BeginTransaction(write_options, txn_options);

    for (Transaction &transaction : block->transactions) {
        transaction.setBlockId(block->getIndex());
        const rocksdb::Snapshot* snapshot = txn->GetSnapshot();
        read_options.snapshot = snapshot;
        std::string recipient;

        if (transaction.type != CREATE_TOKEN) {
            s = txn->Get(read_options, handles[4], rocksdb::Slice(transaction.to),&recipient); // looking for account and it's balance
            if (s.IsNotFound()) {
                WalletAccount walletAccount = WalletAccount(transaction.to, 0, {});
                recipient = walletAccount.to_json_string();
                s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(recipient));
            }
        }

        if (transaction.type == UNIT_TRANSFER)
            goto unit_transfer;
        else if (transaction.type == CREATE_TOKEN)
            goto create_token;
        else if (transaction.type == TOKEN_TRANSFER)
            goto transfer_tokens;
        else
            return false;


        unit_transfer: {
            boost::json::object recipient_json = boost::json::parse(recipient).as_object();
            s = txn->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.from), &recipient); // looking for account and it's balance

            // for genesis comment under
            boost::json::object sender_json = boost::json::parse(recipient).as_object();
            if(!sender_json.contains("amount") || (boost::json::value_to<double>(sender_json["amount"]) < transaction.amount)) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }
            // for genesis comment above

            sender_json["amount"] = boost::json::value_to<double>(sender_json["amount"]) - transaction.amount; // for genesis comment this
            recipient_json["amount"] = boost::json::value_to<double>(recipient_json["amount"]) + transaction.amount;
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(sender_json))); // for genesis comment this
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
            goto push_tx;
        };

        create_token: {
            boost::json::object transaction_parser = boost::json::parse(transaction.to_json_string()).as_object();
            if (!transaction_parser["extradata"].as_object().contains("bytecode")) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }

            std::string hex = boost::json::value_to<std::string>(transaction_parser["extradata"].at("bytecode"));
            boost::json::object bytecode_parsed;
            try {
                bytecode_parsed = boost::json::parse(hex_to_ascii(hex)).as_object();
            } catch (std::exception &e) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }

            std::string token;
            s = txn->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(serialize(bytecode_parsed["name"])), &token); // looking for token
            if(!token.empty()) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }

            Token token_created = Token(boost::json::value_to<std::string>(bytecode_parsed["name"]), transaction.extra_data["bytecode"], transaction.from, boost::json::value_to<double>(bytecode_parsed["supply"]));
            s = txn->PutUntracked(handles[1], rocksdb::Slice(token_created.name), rocksdb::Slice(token_created.to_json_string()));
            transaction.setTo(token_created.token_hash);

            s = txn->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.from), &recipient); // looking for account and it's balance
            boost::json::object creator = boost::json::parse(recipient).as_object();
            boost::json::object prepared_token_json;
            prepared_token_json.emplace(token_created.name, token_created.supply);
            creator["tokens_balance"].as_array().emplace_back(prepared_token_json);
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(creator)));
            goto push_tx;
        };

        transfer_tokens: {
            std::string token;
            s = txn->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(transaction.extra_data["name"]), &token); // looking for token
            if(token.empty()) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }

            boost::json::object recipient_json = boost::json::parse(recipient).as_object();

            bool balance_in_token = false;
            for(boost::json::array::iterator it = recipient_json.at("tokens_balance").as_array().begin(); it != recipient_json.at("tokens_balance").as_array().end(); ++it){
                if(it->as_object().contains(transaction.extra_data["name"])) {
                    it->as_object()[transaction.extra_data["name"]] = boost::json::value_to<double>(it->at(transaction.extra_data["name"])) + std::stod(transaction.extra_data["value"]);
                    balance_in_token = true;
                }
            }

            if (!balance_in_token) {
                boost::json::object prepared_token_json;
                prepared_token_json.emplace(transaction.extra_data["name"], std::stod(transaction.extra_data["value"]));
                recipient_json["tokens_balance"].as_array().emplace_back(prepared_token_json);
            }

            std::string sender;
            s = txn->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.from), &sender); // looking for token

            if (sender.empty()) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }

            boost::json::object sender_json = boost::json::parse(sender).as_object();
            balance_in_token = false;
            for(boost::json::array::iterator it = sender_json.at("tokens_balance").as_array().begin(); it != sender_json.at("tokens_balance").as_array().end(); ++it){
                if(it->as_object().contains(transaction.extra_data["name"])) {
                    if ((boost::json::value_to<double>(it->at(transaction.extra_data["name"])) < std::stod(transaction.extra_data["value"]))) {
                        block->transactions.erase(
                                std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                                block->transactions.end());
                        goto leave;
                    }
                    it->as_object()[transaction.extra_data["name"]] = boost::json::value_to<double>(it->at(transaction.extra_data["name"])) - std::stod(transaction.extra_data["value"]);
                    balance_in_token = true;
                }
            }

            if (!balance_in_token) {
                block->transactions.erase(
                        std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                        block->transactions.end());
                goto leave;
            }
//boost::json::value_to<std::string>(
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(sender_json)));

            goto push_tx;
        };

        push_tx:{
            transaction.generate_tx_hash();
            s = txn->PutUntracked(handles[2], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string()));
        };

        leave:{ snapshot = nullptr; };
    }

    s = txn->Commit();

    await: {
        if (s.IsBusy())
            goto await;
    };

    for (auto &handle : handles)
        txn_db->DestroyColumnFamilyHandle(handle);

    delete txn;
    delete txn_db;

    return true;
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

bool unit::DB::validate_sender_balance(Transaction *transaction) {
    std::optional<std::string> op_sender = unit::DB::get_balance(transaction->from);
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::Open(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    if (!op_sender.has_value()) {
        close_db(db, &handles);
        return false;
    }

    if (transaction->type == UNIT_TRANSFER)
        goto unit_transfer;
    else if (transaction->type == CREATE_TOKEN) {
        close_db(db, &handles);
        goto create_token;
    } else if (transaction->type == TOKEN_TRANSFER)
        goto transfer_tokens;
    else
        return false;

    unit_transfer: {
    nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
    parsed_wallet["amount"] = parsed_wallet["amount"].get<double>() - transaction->amount;
    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction->from),rocksdb::Slice(parsed_wallet.dump()));

    close_db(db, &handles);
    if (!status.ok())
        return false;
    return true;
};

    create_token: {
    std::optional op_token = unit::DB::create_new_token(transaction);
    if (!op_token.has_value())
        return false;
    transaction->setTo(op_token.value());
    return true;
};

    transfer_tokens: {
    std::optional op_token = unit::DB::get_token(transaction->extra_data["name"]);
    nlohmann::json parsed_wallet = nlohmann::json::parse(op_sender.value());
    bool balance_in_token = false;
    for (nlohmann::json::iterator it = parsed_wallet["tokens_balance"].begin(); it != parsed_wallet["tokens_balance"].end(); ++it) {
        if (it.value().contains(transaction->extra_data["name"])) { // if user already has balance in current tokens
            it.value()[transaction->extra_data["name"]] = it.value()[transaction->extra_data["name"]].get<double>() - std::stod(transaction->extra_data["value"]);
            balance_in_token = true;
        }
    }

    if (!balance_in_token) {
        close_db(db, &handles);
        return false;
    }

    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice(transaction->from),rocksdb::Slice(parsed_wallet.dump()));

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