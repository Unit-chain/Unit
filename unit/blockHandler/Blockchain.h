//
// Created by Kirill Zhukov on 01.11.2022.
//

#ifndef UNIT_BLOCKCHAIN_H
#define UNIT_BLOCKCHAIN_H

#include <iostream>
#include <utility>
#include <boost/shared_ptr.hpp>
#include "thread"
#include "cassert"
#include "random"
#include "deque"
#include "boost/unordered_map.hpp"
#include "boost/unordered/unordered_set.hpp"

#include "TransactionProcessor.h"
#include "GlobalVariables.h"
#include "db/DB.h"
#include "blockchain/block/BlockBuilder.h"
#include "account/WalletAccount.h"
#include "account/Token.h"
#include "ecdsa.hpp"
#include "utils.hpp"
#include "BIP44.hpp"
#include "BIP39.hpp"
#include "concurrency/DBWriter.h"
#include <boost/smart_ptr.hpp>

template<class T>
using serverCallback = void* (*)(T *server);

class Blockchain {
public:
    inline explicit Blockchain(std::string &blockPath, std::string &userPath, std::string &transactionPath,
                               std::string &historyPath, std::string &tokenPath, BIP44Result &bip44Result,
                               unit::list<Shard> *shardList, boost::shared_ptr<unit::vector<std::string>> &queue)
            : blockWriter(blockPath),
              userWriter(userPath),
              transactionWriter(transactionPath),
              tokenWriter(tokenPath),
              prover(bip44Result),
              blockBuilder(bip44Result),
              historyWriter(historyPath),
              shardList(shardList),
              explorer_queue(queue)
              {}
private:
    unit::BasicDB blockWriter;
    unit::BasicDB userWriter;
    unit::BasicDB transactionWriter;
    unit::BasicDB historyWriter;
    unit::BasicDB tokenWriter;
    unit::list<Shard> *shardList;
    boost::shared_ptr<unit::vector<std::string>> explorer_queue;
    Block *previousBlock;
    Block *currentBlock;
    BlockBuilder blockBuilder;
    BIP44Result prover;
    Addresses addressesUtils;
    std::string currentBlockKey = "current";
public:
//    template<class T>
//    [[noreturn]] void start(serverCallback<T> serverCB);
    [[noreturn]] void start();
};

//template<class T>
void Blockchain::start() {
    printf("%lu: [INFO] --- 'block generator' has been started\n", time(nullptr));
    std::string addressP = this->addressesUtils.pub2addr(this->prover.pub);
    std::string parentHash{};
    if (this->currentBlock == nullptr) currentBlock = new Block();
    while (true) {
        if (this->previousBlock != nullptr) {
            std::optional<Shard> opShard = (!shardList->empty()) ? std::optional<Shard>(shardList->front())
                                                                 : std::nullopt;
            uint64_t index = ++this->previousBlock->blockHeader.index;
            this->blockBuilder.setBlock(this->currentBlock)
                    ->setPreviousHash(this->previousBlock->blockHeader.hash)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(
                            std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                    ->setIndex(index)
                    ->setNonce(0) /// while no consensus implemented
                    ->setEpoch("0x0") /// while no consensus implemented
                    ->setBlockHeader()
                    ->setMessage("test block")
                    ->setRewardProverAddress(addressP)
                    ->insertShard(opShard)
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->build();
            if (opShard.has_value()) shardList->pop_back();
        } else {
            try {
                std::string dbResponse = this->blockWriter.getWithIO(currentBlockKey);
                boost::json::value blockFromDB = boost::json::parse(dbResponse);
                parentHash = boost::json::value_to<std::string>(blockFromDB.at("hash"));
                this->blockBuilder.setBlock(this->currentBlock)
                        ->setPreviousHash(parentHash)
                        ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count())
                        ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                        ->setIndex(boost::json::value_to<uint64_t>(blockFromDB.at("index")))
                        ->setNonce(0) /// while no consensus implemented
                        ->setEpoch("0x0") /// while no consensus implemented
                        ->setBlockHeader()
                        ->setMessage("test block")
                        ->setRewardProverAddress(addressP)
                        ->generateHash()
                        ->generateRoot()
                        ->setProverSign()
                        ->build();
            } catch (std::exception &e) {
                parentHash = "genesis";
                std::optional<Shard> opShard = (!shardList->empty()) ? std::optional<Shard>(shardList->front())
                                                                     : std::nullopt;
                unit::vector<Transaction> genesisList{};
                Transaction rawTransaction = Transaction::parseToGenesis(R"({"from": "UNTCoinbase","to": "UNTwLjJNFd7zuoGBzTM5jvAPjdLAwmg","amount": "0x55730","type": 0,"signature": "null","r": "null","s": "null","nonce": 0,"fee":0})");
                rawTransaction.generateHash();
                genesisList.emplace_back(rawTransaction);
                Shard genesisShard = Shard(genesisList);
                this->currentBlock->emplaceBack(genesisShard);
                this->blockBuilder.setBlock(this->currentBlock)
                        ->setPreviousHash(parentHash)
                        ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count())
                        ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                        ->setIndex(0)
                        ->setNonce(0) /// while no consensus implemented
                        ->setEpoch("0x0") /// while no consensus implemented
                        ->setBlockHeader()
                        ->setMessage("test block")
                        ->setRewardProverAddress(addressP)
                        ->insertShard(opShard)
                        ->generateHash()
                        ->generateRoot()
                        ->setProverSign()
                        ->build();
            }
        }
        std::shared_ptr<rocksdb::WriteBatch> txBatch = unit::BasicDB::getBatch();
        std::thread txThread(TransactionProcessor::setTxBatch, this->currentBlock, txBatch.get(),
                             &this->userWriter, &this->tokenWriter, &this->historyWriter, &(this->currentBlock->blockHeader.size));
        std::shared_ptr<rocksdb::WriteBatch> blockBatchPtr = DBWriter::getBatch();
        txThread.join();
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlock->blockHeader.hash), rocksdb::Slice(this->currentBlock->serializeBlock()));
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlockKey), rocksdb::Slice(this->currentBlock->serializeBlock()));
        this->blockWriter.commit(blockBatchPtr);
        this->explorer_queue->emplace_back(this->currentBlock->serializeBlock());
        if (txBatch->HasPut()) this->transactionWriter.commit(txBatch);
        printf("%lu: [INFO] --- block: %s\n", time(nullptr), this->currentBlock->serializeBlock().c_str());
        this->previousBlock = nullptr;
        delete this->previousBlock;
        this->previousBlock = currentBlock;
        this->currentBlock = new Block();
        std::this_thread::sleep_for(std::chrono::seconds(9));
    }
}

#endif //UNIT_BLOCKCHAIN_H