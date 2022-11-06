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
#include "../global/GlobalVariables.h"
#include "../libdevcore/datastructures/concurrency/DBWriter.h"
#include "../libdevcore/datastructures/blockchain/block/Block.h"
#include "../libdevcore/datastructures/blockchain/block/BlockBuilder.h"
#include "../libdevcore/datastructures/account/WalletAccount.h"
#include "../libdevcore/datastructures/account/Token.h"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../libdevcore/bip44/utils.hpp"
#include "../libdevcore/bip44/BIP44.hpp"
#include "../libdevcore/bip44/BIP39.hpp"

void bar(int x)
{
    std::cout << x << std::endl;
}

class DBStaticBatcher {
public:
    static void setTxBatch(unit::list<ValidTransaction> *txList, rocksdb::WriteBatch *writeBatch,
                           dbProvider::BatchProvider *userDbProvider, dbProvider::BatchProvider *tokensDbProvider, uint64_t *blockSize);
};
void DBStaticBatcher::setTxBatch(unit::list<ValidTransaction> *txList, rocksdb::WriteBatch *writeBatch, dbProvider::BatchProvider *userDbProvider,
                                 dbProvider::BatchProvider *tokensDbProvider, uint64_t *blockSize) {
    *blockSize = 0;
    boost::unordered_map<std::string, std::shared_ptr<WalletAccount>> cache = boost::unordered_map<std::string, std::shared_ptr<WalletAccount>>(txList->size()); //
    std::vector<ValidTransaction> valuesToBeDeleted;
    std::vector<std::string> accounts{};
    accounts.reserve(txList->size());
    bool creatingTokensTx = false;
    for (auto &it : *txList) {
        if (std::find(accounts.begin(), accounts.end(), it.from) != accounts.end()) accounts.emplace_back(it.from);
        if (std::find(accounts.begin(), accounts.end(), it.to) != accounts.end()) accounts.emplace_back(it.to);
        if (it.type == CREATE_TOKENS) creatingTokensTx = true;
    }
    auto response = userDbProvider->multiRead(&accounts);
    assert(response.error == false);

    /// accounts = statuses = responses = accounts.size()
    /// cause database returning statuses 1:1 to keys
    /// each account address is key
    /// that's the reason for them to have the same size
    for (int i = 0; i < response.statuses.size(); ++i) {
        if ((uint16_t) response.statuses.at(i).code() == 0) cache.emplace(accounts.at(i), WalletAccount::parseWallet(&response.responses.at(i)).value());
        else cache.emplace(accounts.at(i), WalletAccount::createEmptyWallet(accounts.at(i)));
    }

    std::shared_ptr<rocksdb::WriteBatch> tokenBatch{};
    if (creatingTokensTx) tokenBatch = dbProvider::BatchProvider::getBatch();
    for (auto &it : *txList) {
        if (!cache.contains(it.from) || !cache.contains(it.to)) { valuesToBeDeleted.emplace_back(it); continue; }
        std::shared_ptr<WalletAccount> senderAccount = cache.at(it.from);
        std::shared_ptr<WalletAccount> recipientAccount = cache.at(it.to);

        if (it.type == TRANSFER && (senderAccount->balance < it.amount)) { valuesToBeDeleted.emplace_back(it); continue; }
        else { goto transfer; }
        if (it.type == TRANSFER_TOKENS && ((!senderAccount->tokensBalance.contains(boost::json::value_to<std::string>(it.extra.at("name")))) ||
                                           (boost::json::value_to<double>(it.extra.at("value")) < boost::json::value_to<double>(it.extra.at("value"))))) { valuesToBeDeleted.emplace_back(it); continue; }
        else { goto transfer_tokens; }
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
        create_token:{
            std::optional<std::shared_ptr<Token>> tokenOptional = Token::parse(&it.extra);
            if (!tokenOptional.has_value()) { valuesToBeDeleted.emplace_back(it); continue; }
            std::shared_ptr<Token> token = tokenOptional.value();
            senderAccount->increaseToken(token->supply, it.hash, token->name);
            tokenBatch->Put(rocksdb::Slice(token->name), rocksdb::Slice(token->serialize()));
            std::shared_ptr<std::string> serializedStr = it.serializeToJsonTransaction();
            writeBatch->Put(rocksdb::Slice(it.hash), rocksdb::Slice(*serializedStr));
            *blockSize += serializedStr->size();
            continue;
        }
    }
    for (auto& [key, value]: cache) writeBatch->Put(rocksdb::Slice(key), rocksdb::Slice(value->serialize()));
    for (auto &valueToBeRemoved : valuesToBeDeleted) txList->remove(valueToBeRemoved);
    if (creatingTokensTx) tokensDbProvider->commitBatch(tokenBatch);
}

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
    DBWriter blockWriter;
    DBWriter userWriter;
    DBWriter transactionWriter;
    DBWriter historyWriter;
    DBWriter tokenWriter;
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
        if (this->previousBlock == nullptr) {
            operationDBStatus::DBResponse<std::string> dbResponse = blockWriter.getProvider()->read<std::string>(&currentBlockKey);
            if (dbResponse.error) { assert((int) dbResponse.errorResponse == 1); }
            this->blockBuilder
                    .setPreviousHash(((int) dbResponse.errorResponse == 1) ? "genesis" : *dbResponse.value)
                    ->setMessage("test blocks")
                    ->setRewardProverAddress(addressP)
                    ->setTime(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
                    ->setVersion(1) /// previous version was implemented earlier. Now it's second version, e.g 1
                    ->setIndex(((int) dbResponse.errorResponse == 1) ? 0 : boost::json::value_to<uint64_t>(boost::json::parse(*dbResponse.value).at("index")))
                    ->setNonce(0) /// while no consensus implemented
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
                    ->generateHash()
                    ->generateRoot()
                    ->setProverSign()
                    ->setBlockHeader()
                    ->build();
        }
        *this->lock = true;
    }
    pushingBlock: {
        std::shared_ptr<rocksdb::WriteBatch> txBatch = this->blockWriter.getProvider()->getBatch();
        std::thread txThread(DBStaticBatcher::setTxBatch, &this->currentBlock->txList, txBatch.get(),
                             this->userWriter.getProvider(), this->tokenWriter.getProvider(), &(this->currentBlock->blockHeader.size));
        std::thread first (bar, 10);
        first.join();
        std::shared_ptr<WriteBatch> blockBatchPtr = DBWriter::getBatch();
        blockBatchPtr->Put(rocksdb::Slice(this->currentBlock->blockHeader.hash), rocksdb::Slice(this->currentBlock->serializeBlock()));
        txThread.join();
        this->blockWriter.getProvider()->commitBatch(blockBatchPtr);
        this->transactionWriter.getProvider()->commitBatch(txBatch);
    }
    *this->previousBlock = *currentBlock;
    *this->currentBlock = Block();
    std::this_thread::sleep_for(std::chrono::seconds ( 3));
    goto start;
}

#endif //UNIT_BLOCKHANDLER_H
