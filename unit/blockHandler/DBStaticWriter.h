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
#include "boost/unordered_map.hpp"
#include "rocksdb/db.h"

inline std::mt19937& generator() {
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
    static void setTxBatch(unit::vector<Shard> *shardList, rocksdb::WriteBatch *writeBatch, unit::DB *userDbProvider,
                           unit::DB *tokensDbProvider, uint64_t *blockSize);
};


void DBStaticWriter::setTxBatch(unit::vector<Shard> *shardList, rocksdb::WriteBatch *writeBatch, unit::DB *userDbProvider,
                                unit::DB *tokensDbProvider, uint64_t *blockSize) {
    *blockSize = 0;
    boost::unordered_map<std::string, std::shared_ptr<WalletAccount>> cache = boost::unordered_map<std::string, std::shared_ptr<WalletAccount>>(shardList->at(
            mersenneRand(0, (shardList->size()-1))).transactionList.size()); // randomly chose pre-allocated size of map
    std::shared_ptr<rocksdb::WriteBatch> tokenBatch{};
    bool creatingTokensTx = false;
    for (auto &shard : *shardList) {
        std::vector<ValidTransaction> valuesToBeDeleted;
        std::vector<std::string> accounts{};
        accounts.reserve(shardList->size());
        for (auto &tx: shard.transactionList) {
            if (std::find(accounts.begin(), accounts.end(), tx.from) != accounts.end())
                accounts.emplace_back(tx.from);
            if (std::find(accounts.begin(), accounts.end(), tx.to) != accounts.end()) accounts.emplace_back(tx.to);
            if (tx.type == CREATE_TOKENS) creatingTokensTx = true;
        }
        auto response = userDbProvider->multiGet(&accounts);
        assert(std::holds_alternative<std::exception>(response) == false);

        /// accounts = statuses = responses = accounts.size()
        /// cause database returning statuses 1:1 to keys
        /// each account address is key
        /// that's the reason for them to have the same size
        std::vector<rocksdb::Status> statuses = std::get<0>(std::get<0>(response));
        std::vector<std::string> responses = std::get<1>(std::get<0>(response));
        for (int i = 0; i < statuses.size(); ++i) {
            if ((uint16_t) statuses.at(i).code() == 0)
                cache.emplace(accounts.at(i), WalletAccount::parseWallet(&responses.at(i)).value());
            else cache.emplace(accounts.at(i), WalletAccount::createEmptyWallet(accounts.at(i)));
        }

        if (creatingTokensTx) tokenBatch = unit::DB::getBatch();
        for (auto &it: shard.transactionList) {
            if (!cache.contains(it.from) || !cache.contains(it.to)) {
                valuesToBeDeleted.emplace_back(it);
                continue;
            }
            std::shared_ptr<WalletAccount> senderAccount = cache.at(it.from);
            std::shared_ptr<WalletAccount> recipientAccount = cache.at(it.to);

            if (it.type == TRANSFER && (senderAccount->balance < it.amount)) {
                if (senderAccount->balance < it.amount) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                } else goto transfer;
            }
            if (it.type == TRANSFER_TOKENS ) {
                if ((!senderAccount->tokensBalance.contains(
                        boost::json::value_to<std::string>(it.extra.at("name")))) ||
                    (boost::json::value_to<double>(it.extra.at("value")) <
                     boost::json::value_to<double>(it.extra.at("value")))) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                } else goto transfer_tokens;
            }
            if (it.type == CREATE_TOKENS) goto create_token;

            transfer: {
                senderAccount->subtract(it.amount, it.hash);
                recipientAccount->increase(it.amount, it.hash);
                std::shared_ptr<std::string> serializedStr = it.serializeToJsonTransaction();
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(*serializedStr));
                *blockSize += serializedStr->size();
                continue;
            };
            transfer_tokens: {
                auto tokenName = boost::json::value_to<std::string>(it.extra.at("name"));
                senderAccount->subtractToken(it.amount, it.hash, tokenName);
                senderAccount->increaseToken(it.amount, it.hash, tokenName);
                std::shared_ptr<std::string> serializedStr = it.serializeToJsonTransaction();
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(*serializedStr));
                *blockSize += serializedStr->size();
                continue;
            };
            create_token: {
                std::optional<std::shared_ptr<Token>> tokenOptional = Token::parse(&it.extra);
                if (!tokenOptional.has_value()) {
                    valuesToBeDeleted.emplace_back(it);
                    continue;
                }
                std::shared_ptr<Token> token = tokenOptional.value();
                senderAccount->increaseToken(token->supply, it.hash, token->name);
                tokenBatch->Put(rocksdb::Slice(token->name), rocksdb::Slice(token->serialize()));
                std::shared_ptr<std::string> serializedStr = it.serializeToJsonTransaction();
                writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(*serializedStr));
                *blockSize += serializedStr->size();
                continue;
            }
        }
        for (auto &[key, value]: cache) writeBatch->Put(rocksdb::Slice(key), rocksdb::Slice(value->serialize()));
        for (auto &valueToBeRemoved : valuesToBeDeleted) shard.transactionList.remove(valueToBeRemoved);
    }
    if (creatingTokensTx) tokensDbProvider->commit(tokenBatch);
}
#endif //UNIT_DBSTATICWRITER_H
