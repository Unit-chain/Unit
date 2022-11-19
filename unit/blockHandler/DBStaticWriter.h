//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_DBSTATICWRITER_H
#define UNIT_DBSTATICWRITER_H

#include "iostream"
#include <random>
#include "../libdevcore/db/DB.h"
#include "../libdevcore/datastructures/account/Token.h"
#include "../libdevcore/datastructures/containers/vector.h"
#include "../libdevcore/datastructures/account/WalletAccount.h"
#include "../libdevcore/datastructures/blockchain/block/Shard.h"
#include "../libdevcore/datastructures/blockchain/block/Block.h"
#include "boost/unordered_map.hpp"
#include "rocksdb/db.h"

inline std::mt19937 &generator() {
    // the generator will only be seeded once (per thread) since it's static
    static thread_local std::mt19937 gen(std::random_device{}());
    return gen;
}

uint64_t mersenneRand(uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(generator());
}

class DBStaticWriter {
public:
    static void setTxBatch(Block *block, rocksdb::WriteBatch *writeBatch, unit::DB *userDbProvider,
                           unit::DB *tokensDbProvider, unit::DB *historyDBProvider);
};


void DBStaticWriter::setTxBatch(Block *block, rocksdb::WriteBatch *writeBatch, unit::DB *userDbProvider,
                                unit::DB *tokensDbProvider, unit::DB *historyDBProvider) {
    if (block->shardList.empty()) return;
    std::map<std::string, WalletAccount*> cache = std::map<std::string, WalletAccount*>();
    std::shared_ptr<rocksdb::WriteBatch> tokenBatch{};
    std::shared_ptr<rocksdb::WriteBatch> historyBatch = unit::DB::getBatch();
    std::shared_ptr<rocksdb::WriteBatch> userBatch = unit::DB::getBatch();
    bool creatingTokensTx = false;
    for (auto &shard: block->shardList) {
        std::vector<ValidTransaction> valuesToBeDeleted;
        std::vector<std::string> accounts{};
        accounts.reserve(block->shardList.size());
        for (auto &tx: shard.transactionList) {
            if (std::count(accounts.begin(), accounts.end(), tx.from) == 0) accounts.emplace_back(tx.from);
            if (std::count(accounts.begin(), accounts.end(), tx.to) == 0) accounts.emplace_back(tx.to);
            if (tx.type == CREATE_TOKENS) creatingTokensTx = true;
            if (block->blockHeader.index == 0) cache.emplace(tx.to, new WalletAccount(tx.to));
        }

        std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>> response;
        try {
            try {
                response = userDbProvider->multiGet(&accounts);
            } catch (unit::error::DBIOError &e) {
                std::cout << e.what() << std::endl;
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            exit(2);
        }

        /// accounts = statuses = responses = accounts.size()
        /// cause database returning statuses 1:1 to keys
        /// each account address is key
        /// that's the reason for them to have the same size
        std::vector<rocksdb::Status> statuses = std::get<0>(response);
        std::vector<std::string> responses = std::get<1>(response);
        if (block->blockHeader.index != 0) {
            for (int i = 0; i < statuses.size(); ++i) {
                if ((uint16_t) statuses.at(i).code() == 0) cache.emplace(accounts.at(i), WalletAccount::createPtrWallet(responses.at(i)));
                else cache.emplace(accounts.at(i), WalletAccount::createEmptyPtrWallet(accounts.at(i)));
            }
        }

        if (creatingTokensTx) tokenBatch = unit::DB::getBatch();
        for (auto &it: shard.transactionList) {
            if (block->blockHeader.index != 0 && (!cache.contains(it.from) || !cache.contains(it.to))) {
                valuesToBeDeleted.emplace_back(it);
                continue;
            }
            WalletAccount *senderAccount = nullptr;
            if (block->blockHeader.index != 0) {
                senderAccount = cache.at(it.from);
            }
            WalletAccount *recipientAccount = cache.at(it.to);

            if (it.type == TRANSFER) {
                if (block->blockHeader.index == 0) goto transfer;
                if (senderAccount->balance < it.amount) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                } else goto transfer;
            }
            if (it.type == TRANSFER_TOKENS) {
                if ((!senderAccount->tokensBalance.as_object().contains(
                        boost::json::value_to<std::string>(it.extra.at("name")))) ||
                    (boost::json::value_to<double>(it.extra.at("value")) <
                     boost::json::value_to<double>(it.extra.at("value")))) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                } else goto transfer_tokens;
            }
            if (it.type == CREATE_TOKENS) goto create_token;

            transfer:
            {
                std::string serializedStr = it.serializeToJsonTransaction();
                if (block->blockHeader.index != 0) {
                    try {
                        std::shared_ptr<rocksdb::DB*> sharedDB = historyDBProvider->newReadOnlyDB();
                        senderAccount->parseHistory(historyDBProvider->get(senderAccount->address, sharedDB));
                        recipientAccount->parseHistory(historyDBProvider->get(senderAccount->address, sharedDB));
                        unit::DB::close(sharedDB);
                    } catch (std::exception &e) {
                        std::cout << e.what() << std::endl;
                    }
                    senderAccount->subtract(it.amount, it.hash);
                    historyBatch->Put(rocksdb::Slice(senderAccount->address), rocksdb::Slice(senderAccount->serializeHistory()));
                }
                recipientAccount->increase(it.amount, it.hash);
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(serializedStr));
                historyBatch->Put(rocksdb::Slice(recipientAccount->address), rocksdb::Slice(recipientAccount->serializeHistory()));
                block->blockHeader.size += serializedStr.size();
                continue;
            };
            transfer_tokens:
            {
                auto tokenName = boost::json::value_to<std::string>(it.extra.at("name"));
                senderAccount->subtractToken(it.amount, it.hash, tokenName);
                senderAccount->increaseToken(it.amount, it.hash, tokenName);
                std::string serializedStr = it.serializeToJsonTransaction();
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(serializedStr));
                block->blockHeader.size += serializedStr.size();
                continue;
            };
            create_token:
            {
                std::optional<std::shared_ptr<Token>> tokenOptional = Token::parse(&it.extra);
                if (!tokenOptional.has_value()) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                }
                std::shared_ptr<Token> token = tokenOptional.value();
                senderAccount->increaseToken(token->supply, it.hash, token->name);
                tokenBatch->Put(rocksdb::Slice(token->name), rocksdb::Slice(token->serialize()));
                std::string serializedStr = it.serializeToJsonTransaction();
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(serializedStr));
                block->blockHeader.size += serializedStr.size();
                continue;
            }
        }
        boost::unordered::unordered_map<std::string, std::shared_ptr<WalletAccount>>::iterator it;
        for (auto &[key, value]: cache)  {
            userBatch->Put(rocksdb::Slice(key), rocksdb::Slice(value->serialize()));
            value = nullptr;
            delete value;
        }
        for (auto &valueToBeRemoved: valuesToBeDeleted) shard.transactionList.remove(valueToBeRemoved);
    }
    if (creatingTokensTx) tokensDbProvider->commit(tokenBatch);
    historyDBProvider->commit(historyBatch);
    std::optional<std::exception> except = userDbProvider->commit(userBatch);
    if (except.has_value()) std::cout << "batch is: " <<   except.value().what() << std::endl;
    else std::cout << "batch is: ok" << std::endl;
}

#endif //UNIT_DBSTATICWRITER_H
