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
                                 std::string &historyPath, std::string &tokenPath, std::string &mnemonic, std::string &passphrase)
    : blockWriter(blockPath),
      userWriter(userPath),
      transactionWriter(transactionPath),
      tokenWriter(tokenPath),
      historyWriter(historyPath) {
        BIP39Result bip39Result = BIP39Result(mnemonic, passphrase);
        BIP44 bip44 = BIP44();
        BIP44Result bip44Result = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, 0);
        this->blockBuilder = BlockBuilder(bip44Result);
        this->bip44Result = bip44Result;
        *this->lock = false;
    }
    [[noreturn]] void startBlockGenerator();
private:
    unit::BasicDB blockWriter;
    unit::BasicDB userWriter;
    unit::BasicDB transactionWriter;
    unit::BasicDB historyWriter;
    unit::BasicDB tokenWriter;
    unit::list<ValidTransaction> transactions_deque;
    Block *previousBlock;
    Block *currentBlock;
    BlockBuilder blockBuilder;
    BIP44Result bip44Result;
    Addresses addressesUtils;
    std::string currentBlockKey = "current";
    bool *lock;
};

void BlockHandler::startBlockGenerator() {
    std::cout << "Starting 'block generator'" << std::endl;
    std::string addressP =  this->addressesUtils.pub2addr(this->bip44Result.pub);
    this->blockBuilder.setBlock(this->currentBlock);
    start: {
        std::string parentHash{};
        if (this->previousBlock == nullptr) {
            std::variant<std::string, std::exception> dbResponse = this->blockWriter.get(currentBlockKey);
            if (std::holds_alternative<std::exception>(dbResponse)) {
                if (unit::DB::isSameError<decltype(std::get<1>(dbResponse)), unit::error::DBNotFound>(std::get<1>(dbResponse)))
                    parentHash = "genesis";
            } else parentHash = std::get<0>(dbResponse);

            this->blockBuilder
                    .setPreviousHash(parentHash)
                    ->setMessage("test blocks")
                    ->setRewardProverAddress(addressP)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                    ->setIndex((parentHash == "genesis") ? 0 : boost::json::value_to<uint64_t>(boost::json::parse(std::get<0>(dbResponse)).at("index")))
                    ->setNonce(0) /// while no consensus implemented
                    ->setEpoch(0) /// while no consensus implemented
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->setBlockHeader()
                    ->build();
        } else {
             this->blockBuilder
                    .setPreviousHash(this->previousBlock->blockHeader.hash)
                    ->setMessage("test blocks") /// need much more tests
                    ->setRewardProverAddress(addressP)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1)
                    ->setIndex(++(this->previousBlock->blockHeader.index))
                    ->setNonce(0) /// while no consensus implemented
                    ->setEpoch(0) /// while no consensus implemented
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->setBlockHeader()
                    ->build();
        }
        *this->lock = true;
    }
    pushingBlock: {
        std::shared_ptr<rocksdb::WriteBatch> txBatch = unit::BasicDB::getBatch();
        std::thread txThread(DBStaticWriter::setTxBatch, &this->currentBlock->shardList, txBatch.get(),
                             &this->userWriter, &this->tokenWriter, &(this->currentBlock->blockHeader.size));
        std::shared_ptr<rocksdb::WriteBatch> blockBatchPtr = DBWriter::getBatch();
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlock->blockHeader.hash), rocksdb::Slice(this->currentBlock->serializeBlock()));
        txThread.join();
        this->blockWriter.commit(blockBatchPtr);
        this->transactionWriter.commit(txBatch);
    }
    *this->previousBlock = *currentBlock;
    *this->currentBlock = Block();
    std::this_thread::sleep_for(std::chrono::seconds ( 3));
    goto start;
}

#endif //UNIT_BLOCKHANDLER_H
