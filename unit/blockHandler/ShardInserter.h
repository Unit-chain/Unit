//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_SHARDINSERTER_H
#define UNIT_SHARDINSERTER_H
#include "iostream"
#include "../libdevcore/datastructures/blockchain/block/Shard.h"
#include "../libdevcore/datastructures/blockchain/block/Block.h"
#include "../libdevcore/bip44/BIP44.hpp"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../pools/TransactionPool.h"

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
        #if 0
                if (transactionPool->getPoolSize() == 0) continue;
                unit::vector<ValidTransaction> transactions{};
                transactions.reserve(transactionPool->getPoolSize());
                Shard shard1{};
                while (!shardList->empty()) {
                    uint64_t lastTx = this->transactionPool->getPoolSize();
                    transactions.assign(transactionPool->getTxPool()->begin(), transactionPool->getTxPool()->end());
                    this->transactionPool->dropTransactions(lastTx);
                }
                shard1.insertList(transactions);
                ECDSASignResult sig = ecdsa_sign_message(shard1.serializeWithoutSignatures(), this->prover.prv);
                shard1.setShardId(prover.address)->setRp(sig.r)->setSp(sig.s)->setSignature(sig.message_hash);
                shardList->emplace_back(shard1);
                std::destroy(transactions.begin(), transactions.end());
        //        this->transactionPool->dropTransactions(this->transactionPool->getPoolSize());
        //        std::this_thread::sleep_for(std::chrono::seconds(5));
        #endif
        if (transactionPool->getPoolSize() == 0) continue;
        unit::vector<Transaction> transactions{};
        transactions.reserve(transactionPool->getPoolSize());
//        std::copy(transactions.begin(), transactions.end(), std::back_inserter(*transactionPool->getTxPool()));
        transactions.assign(transactionPool->getTxPool()->begin(), transactionPool->getTxPool()->end());
//        Shard shard1 = Shard(transactions);
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
