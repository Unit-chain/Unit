//
// Created by Kirill Zhukov on 01.11.2022.
//

#ifndef UNIT_BLOCKHANDLER_H
#define UNIT_BLOCKHANDLER_H
#include <iostream>
#include <utility>
#include "cassert"
#include "random"
#include "deque"
#include "../libdevcore/datastructures/concurrency/DBWriter.h"
#include "../libdevcore/datastructures/blockchain/block/Block.h"
#include "../libdevcore/datastructures/blockchain/block/BlockBuilder.h"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../libdevcore/bip44/utils.hpp"
#include "../libdevcore/bip44/BIP44.hpp"
#include "../libdevcore/bip44/BIP39.hpp"

class BlockHandler {
public:
    inline explicit BlockHandler(std::string &blockPath, std::string &userPath, std::string &transactionPath, std::string &historyPath, std::string &mnemonic, std::string &passphrase)
    : blockWriter(blockPath),
      userWriter(userPath),
      transactionWriter(transactionPath),
      historyWriter(historyPath) {
        BIP39Result bip39Result = BIP39Result(mnemonic, passphrase);
        BIP44 bip44 = BIP44();
        BIP44Result bip44Result = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, 0);
        this->blockBuilder = BlockBuilder(bip44Result);
        this->bip44Result = bip44Result;
    }
    [[noreturn]] void startBlockGenerator();
private:
    DBWriter blockWriter;
    DBWriter userWriter;
    DBWriter transactionWriter;
    DBWriter historyWriter;
    unit::list<ValidTransaction> transactions_deque;
    Block *previousBlock;
    Block *currentBlock;
    BlockBuilder blockBuilder;
    BIP44Result bip44Result;
    Addresses addressesUtils;
    std::string currentBlockKey = "current";
};

void BlockHandler::startBlockGenerator() {
    std::cout << "Starting 'block generator'" << std::endl;
    std::string addressP =  addressesUtils.pub2addr(this->bip44Result.pub);
    Block block;
    start: {
        blockBuilder.setBlock(&block);
        if (this->previousBlock == nullptr) {
            operationDBStatus::DBResponse<std::string> dbResponse = blockWriter.getProvider()->read<std::string>(&currentBlockKey);
            if (dbResponse.error) assert((int) dbResponse.errorResponse == 2);
            block = *blockBuilder
                    .setPreviousHash(((int) dbResponse.errorResponse == 2) ? "genesis" : *dbResponse.value)
                    ->setMessage("test blocks")
                    ->setRewardProverAddress(addressP)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                    ->setIndex(((int) dbResponse.errorResponse == 2) ? 0 : boost::json::value_to<uint64_t>(boost::json::parse(*dbResponse.value).at("index")))
                    ->setNonce(0) /// while no consensus implemented
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->setBlockHeader()
                    ->build();
        } else {
            block = *blockBuilder
                    .setPreviousHash(previousBlock->blockHeader.hash)
                    ->setMessage("test blocks")
                    ->setRewardProverAddress(addressP)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1)
                    ->setIndex(++(previousBlock->blockHeader.index))
                    ->setNonce(0) /// while no consensus implemented
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->setBlockHeader()
                    ->build();
        }
    }
}


#endif //UNIT_BLOCKHANDLER_H
