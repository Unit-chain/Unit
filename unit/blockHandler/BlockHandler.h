//
// Created by Kirill Zhukov on 01.11.2022.
//

#ifndef UNIT_BLOCKHANDLER_H
#define UNIT_BLOCKHANDLER_H
#include <iostream>
#include <utility>
#include "thread"
#include "cassert"
#include "random"
#include "deque"
#include "boost/unordered_map.hpp"
#include "boost/unordered/unordered_set.hpp"

#include "DBStaticWriter.h"
#include "../global/GlobalVariables.h"
#include "../libdevcore/datastructures/concurrency/DBWriter.h"
#include "../libdevcore/db/DB.h"
#include "../libdevcore/datastructures/blockchain/block/BlockBuilder.h"
#include "../libdevcore/datastructures/account/WalletAccount.h"
#include "../libdevcore/datastructures/account/Token.h"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../libdevcore/bip44/utils.hpp"
#include "../libdevcore/bip44/BIP44.hpp"
#include "../libdevcore/bip44/BIP39.hpp"

class BlockHandler {
public:
    inline explicit BlockHandler(std::string &blockPath, std::string &userPath, std::string &transactionPath,
                                 std::string &historyPath, std::string &tokenPath, BIP44Result bip44Result, unit::list<Shard> *shardList, Block *last)
    : blockWriter(blockPath),
      userWriter(userPath),
      transactionWriter(transactionPath),
      tokenWriter(tokenPath),
      prover(bip44Result),
      historyWriter(historyPath),
      shardList(shardList),
      previousBlock(last){
        this->previousBlock = new Block();
//        BIP39Result bip39Result = BIP39Result(mnemonic, passphrase);
//        BIP44 bip44 = BIP44();
//        BIP44Result prover = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, 0);
//        this->blockBuilder = BlockBuilder(bip44Result);
//        this->bip44Result = bip44Result;
    }
    [[noreturn]] void startBlockGenerator();
private:
    unit::BasicDB blockWriter;
    unit::BasicDB userWriter;
    unit::BasicDB transactionWriter;
    unit::BasicDB historyWriter;
    unit::BasicDB tokenWriter;
    unit::list<Shard> *shardList;
    Block *previousBlock;
    Block *currentBlock;
    BlockBuilder blockBuilder;
    BIP44Result prover;
    Addresses addressesUtils;
    std::string currentBlockKey = "current";
};

void BlockHandler::startBlockGenerator() {
    std::cout << "'block generator' has been started" << std::endl;
    std::string addressP =  this->addressesUtils.pub2addr(this->prover.pub);
    if (this->currentBlock == nullptr) currentBlock = new Block();
    this->blockBuilder.setBlock(this->currentBlock);
    start: {
        std::string parentHash{};
        if (this->previousBlock == nullptr) {
            try {
                std::string dbResponse = this->blockWriter.get(currentBlockKey);
                boost::json::value blockFromDB = boost::json::parse(dbResponse);
                parentHash = boost::json::value_to<std::string>(blockFromDB.at(this->currentBlockKey));
                std::shared_ptr<Shard> shardPtr = (!shardList->empty()) ? std::make_shared<Shard>(shardList->front()) : nullptr;
                this->blockBuilder
                        .setPreviousHash(parentHash)
                        ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                        ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                        ->setIndex((parentHash == "genesis") ? 0 : boost::json::value_to<uint64_t>(blockFromDB.at("index")))
                        ->setNonce(0) /// while no consensus implemented
                        ->setEpoch(0) /// while no consensus implemented
                        ->setBlockHeader()
                        ->setMessage("test block")
                        ->setRewardProverAddress(addressP)
                        ->insertShard(shardPtr)
                        ->generateHash()
                        ->generateRoot()
                        ->setProverSign()
                        ->build();
            } catch (std::exception &e) {
                std::shared_ptr<Shard> shardPtr = (!shardList->empty()) ? std::make_shared<Shard>(shardList->front()) : nullptr;
                this->blockBuilder
                        .setPreviousHash(parentHash)
                        ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                        ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                        ->setIndex(0)
                        ->setNonce(0) /// while no consensus implemented
                        ->setEpoch(0) /// while no consensus implemented
                        ->setBlockHeader()
                        ->setMessage("test block")
                        ->setRewardProverAddress(addressP)
                        ->insertShard(shardPtr)
                        ->generateHash()
                        ->generateRoot()
                        ->setProverSign()
                        ->build();
            }
        } else {
            std::shared_ptr<Shard> shardPtr = (!shardList->empty()) ? std::make_shared<Shard>(shardList->front()) : nullptr;
            this->blockBuilder
                    .setPreviousHash(this->previousBlock->blockHeader.hash)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                    ->setIndex(++(this->previousBlock->blockHeader.index))
                    ->setNonce(0) /// while no consensus implemented
                    ->setEpoch(0) /// while no consensus implemented
                    ->setBlockHeader()
                    ->setMessage("test block")
                    ->setRewardProverAddress(addressP)
                    ->insertShard(shardPtr)
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->build();
        }
    }
    pushingBlock: {
        std::shared_ptr<rocksdb::WriteBatch> txBatch = unit::BasicDB::getBatch();
        std::thread txThread(DBStaticWriter::setTxBatch, &this->currentBlock->shardList, txBatch.get(),
                             &this->userWriter, &this->tokenWriter, &(this->currentBlock->blockHeader.size));
        std::shared_ptr<rocksdb::WriteBatch> blockBatchPtr = DBWriter::getBatch();
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlock->blockHeader.hash), rocksdb::Slice(this->currentBlock->serializeBlock()));
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlockKey), rocksdb::Slice(this->currentBlock->serializeBlock()));
        txThread.join();
        this->blockWriter.commit(blockBatchPtr);
        this->transactionWriter.commit(txBatch);
    }
    std::cout << this->previousBlock << std::endl << this->currentBlock << std::endl;
    this->previousBlock = nullptr;
    delete this->previousBlock;
    this->previousBlock = currentBlock;
    std::cout << "block: " << this->currentBlock->serializeBlock() << std::endl;
    this->currentBlock = new Block();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    goto start;
}

#endif //UNIT_BLOCKHANDLER_H
