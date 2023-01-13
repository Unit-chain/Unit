//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_SHARDINSERTER_H
#define UNIT_SHARDINSERTER_H
#include "iostream"
#include "block/Shard.h"
#include "block/Block.h"
#include "bip44/BIP44.hpp"
#include "bip44/ecdsa.hpp"
#include "TransactionPool.h"

class InvalidBlockAccess : public std::exception {
    [[nodiscard]] const char * what() const noexcept override {
        return "invalid access to block";
    }
};

class ShardInserter {
public:
    [[maybe_unused]] virtual void shardFactory() = 0;
};

class BasicLocalShardInserter : public ShardInserter{
public:
    BasicLocalShardInserter(unit::list<Shard> *shardList, TransactionPool *pool, BIP44Result &bip44Result) : shardList(shardList), transactionPool(pool), prover(bip44Result) {}
    [[noreturn]] void shardFactory() override;
private:
    BIP44Result prover;
    unit::list<Shard> *shardList;
    TransactionPool *transactionPool;
};

void BasicLocalShardInserter::shardFactory() {
    while (true) {
        if (transactionPool->getPoolSize() == 0) continue;
        unit::vector<Transaction> transactions{};
        transactions.reserve(transactionPool->getPoolSize());
        transactions.assign(transactionPool->getTxPool()->begin(), transactionPool->getTxPool()->end());
        Shard shard1{};
        shard1.insertList(transactions);
        ECDSASignResult sig = ecdsa_sign_message(shard1.serializeWithoutSignatures(), this->prover.prv);
        shard1.setShardId(prover.address)->setRp(sig.r)->setSp(sig.s)->setSignature(sig.message_hash);
        shardList->emplace_back(shard1);
        this->transactionPool->dropTransactions(this->transactionPool->getPoolSize());
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

#endif //UNIT_SHARDINSERTER_H
