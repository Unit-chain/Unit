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
    options.create_if_missing = true;
    options.create_missing_column_families = true;
    options.max_background_jobs = cpuss;
    options.env->SetBackgroundThreads(cpuss);
    options.num_levels = 2;
    options.merge_operator = nullptr;
    options.compaction_filter = nullptr;
    options.compaction_filter_factory = nullptr;
    options.rate_limiter = nullptr;
    options.max_open_files = -1;
    options.max_write_buffer_number = 6;
    options.max_background_flushes = cpuss;
    options.level0_stop_writes_trigger = -1;
    options.level0_slowdown_writes_trigger = -1;
    options.max_open_files = 5000;
    options.create_if_missing = true;
    options.create_missing_column_families = true;

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

std::optional<std::string> unit::DB::get_block_height() {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    if(db == nullptr) {
        close_db(db, &handles);
        return std::nullopt;
    }

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
    if (!status.ok()) {
        std::cout << "db code: " << status.code() << std::endl;
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);
            std::this_thread::sleep_for(std::chrono::milliseconds( 2000));
        }
    }
    std::string balance;
    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(address), &balance);
    if (!status.ok()) {
        if (status.code() == rocksdb::Status::Code::kNotFound) {
            close_db(db, &handles);
            return std::nullopt;
        }
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(address), &balance);
            std::this_thread::sleep_for(std::chrono::milliseconds( 2000));
        }
    }
    close_db(db, &handles);
    return balance;
}

bool unit::DB::push_transactions(Block *block) {
    rocksdb::Options options;
    rocksdb::DBOptions dbOptions;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::OptimisticTransactionDBOptions optimisticTransactionDbOptions;
    rocksdb::OptimisticTransactionDB* txn_db;
    rocksdb::Status s = rocksdb::OptimisticTransactionDB::Open(unit::DB::get_db_options(), optimisticTransactionDbOptions, kkDBPath, get_column_families(), &handles, &txn_db);
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

        if(block->index == 1) {
            recipient_json["amount"] = boost::json::value_to<double>(recipient_json["amount"]) + transaction.amount;
            recipient_json["inputs"].as_array().emplace_back(transaction.hash);
            s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
            goto push_tx;
        }

        boost::json::object sender_json = boost::json::parse(recipient).as_object();
        if(!sender_json.contains("amount") || (boost::json::value_to<double>(sender_json["amount"]) < transaction.amount)) {
            block->transactions.erase(
                    std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                    block->transactions.end());
            goto leave;
        }

        sender_json["amount"] = boost::json::value_to<double>(sender_json["amount"]) - transaction.amount; // for genesis comment this
        recipient_json["amount"] = boost::json::value_to<double>(recipient_json["amount"]) + transaction.amount;
        recipient_json["inputs"].as_array().emplace_back(transaction.hash);
        sender_json["outputs"].as_array().emplace_back(transaction.hash);

        s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(sender_json))); // for genesis comment this
        s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
        goto push_tx;
    };

        create_token: {
        boost::json::object transaction_parser = boost::json::parse(transaction.to_json_string_test()).as_object();
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

        Token token_created = Token(boost::json::value_to<std::string>(bytecode_parsed["name"]), boost::json::value_to<std::string>(transaction.extra.at("bytecode")), transaction.from, boost::json::value_to<double>(bytecode_parsed["supply"]));
        s = txn->PutUntracked(handles[1], rocksdb::Slice(token_created.name), rocksdb::Slice(token_created.to_json_string()));
        transaction.setTo(token_created.token_hash);

        s = txn->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice(transaction.from), &recipient); // looking for account and it's balance

        boost::json::object creator = boost::json::parse(recipient).as_object();
        boost::json::object prepared_token_json;

        prepared_token_json.emplace(token_created.name, token_created.supply);
        creator["tokens_balance"].as_array().emplace_back(prepared_token_json);

        creator["outputs"].as_array().emplace_back(transaction.hash);
        s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(creator)));
        goto push_tx;
    };

        transfer_tokens: {
        std::string token;

        s = txn->Get(rocksdb::ReadOptions(), handles[1], rocksdb::Slice(boost::json::value_to<std::string>(transaction.extra.at("name"))), &token); // looking for token
        if(token.empty()) {
            block->transactions.erase(
                    std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                    block->transactions.end());
            goto leave;
        }

        boost::json::object recipient_json = boost::json::parse(recipient).as_object();

        bool balance_in_token = false;
        for(boost::json::array::iterator it = recipient_json.at("tokens_balance").as_array().begin(); it != recipient_json.at("tokens_balance").as_array().end(); ++it){
            if(it->as_object().contains(transaction.extra.at("name").as_string())) {
                it->as_object()[transaction.extra.at("name").as_string()] = boost::json::value_to<double>(it->at(transaction.extra.at("name").as_string())) + std::stod(boost::json::value_to<std::string>(transaction.extra.as_object()["value"]));
                balance_in_token = true;
            }
        }

        if (!balance_in_token) {
            boost::json::object prepared_token_json;
            prepared_token_json.emplace(transaction.extra.at("name").as_string(),  std::stod(boost::json::value_to<std::string>(transaction.extra.as_object()["value"])));
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
            if(it->as_object().contains(transaction.extra.at("name").as_string())) {
                if (boost::json::value_to<double>(it->at(transaction.extra.at("name").as_string())) <  std::stod(boost::json::value_to<std::string>(transaction.extra.as_object()["value"]))) {
                    block->transactions.erase(
                            std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                            block->transactions.end());
                    goto leave;
                }
                it->as_object()[transaction.extra.at("name").as_string()] = boost::json::value_to<double>(it->at(transaction.extra.at("name").as_string())) - std::stod(boost::json::value_to<std::string>(transaction.extra.as_object()["value"]));
                balance_in_token = true;
            }
        }

        if (!balance_in_token) {
            block->transactions.erase(
                    std::remove(block->transactions.begin(), block->transactions.end(), transaction),
                    block->transactions.end());
            goto leave;
        }

        recipient_json["inputs"].as_array().emplace_back(transaction.hash);
        sender_json["outputs"].as_array().emplace_back(transaction.hash);
        s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.to), rocksdb::Slice(serialize(recipient_json)));
        s = txn->PutUntracked(handles[4], rocksdb::Slice(transaction.from), rocksdb::Slice(serialize(sender_json)));

        goto push_tx;
    };

        push_tx:{
        s = txn->PutUntracked(handles[2], rocksdb::Slice(transaction.hash), rocksdb::Slice(transaction.to_json_string_test()));
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

bool unit::DB::push_block(Block block) {
    await_when_db_is_not_busy:{};
    rocksdb::Options options;
    rocksdb::DBOptions dbOptions;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::OptimisticTransactionDBOptions optimisticTransactionDbOptions;
    rocksdb::OptimisticTransactionDB* txn_db;
    rocksdb::Status s = rocksdb::OptimisticTransactionDB::Open(unit::DB::get_db_options(), optimisticTransactionDbOptions, kkDBPath, get_column_families(), &handles, &txn_db);
    if(!s.ok())
        goto await_when_db_is_not_busy;

    rocksdb::WriteOptions write_options;
    rocksdb::ReadOptions read_options;
    rocksdb::OptimisticTransactionOptions txn_options;
    std::string value;
    txn_options.set_snapshot = true;
    rocksdb::Transaction* txn = txn_db->BeginTransaction(write_options, txn_options);

    std::string height;
    txn->Get(rocksdb::ReadOptions(), handles[3], rocksdb::Slice("current"), &height);

    if (!height.empty())
        goto common;
    goto genesis;


    genesis: {
    block.setIndex(1);
    block.setPrevHash("genesis");
    goto push_values;
};

    common: {
    boost::json::value parsed_current = boost::json::parse(height);
    block.setPrevHash(boost::json::value_to<std::string>(parsed_current.at("hash")));
    goto push_values;
};

    push_values: {
    std::cout << "block #" << block.getIndex() << ": " << block.to_json_with_tx_hash_only() << std::endl;
    s = txn->PutUntracked(handles[0], rocksdb::Slice(block.hash), rocksdb::Slice(block.to_json_with_tx_hash_only()));
    s = txn->PutUntracked(handles[3], rocksdb::Slice("current"), rocksdb::Slice(block.to_json_with_tx_hash_only()));
};

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


std::optional<std::string> unit::DB::find_transaction(std::string tx_hash) {
    rocksdb::DB *db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(unit::DB::get_db_options(), kkDBPath, unit::DB::get_column_families(), &handles, &db);

    std::string tx;
    status = db->Get(rocksdb::ReadOptions(), handles[2], rocksdb::Slice(std::move(tx_hash)), &tx);

    unit::DB::close_db(db, &handles);
    if(tx.empty())
        return std::nullopt;

    return tx;
}


void unit::DB::close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles) {
    for (auto handle : *handles)
        db->DestroyColumnFamilyHandle(handle);
    delete db;
}