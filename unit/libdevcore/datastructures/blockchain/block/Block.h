//
// Created by Kirill Zhukov on 23.10.2022.
//

#ifndef UNIT_BLOCK_H
#define UNIT_BLOCK_H

#include <utility>

#include "iostream"
#include "sstream"
#include "BlockHeader.h"
#include "../transaction/ValidTransaction.h"
#include "../../containers/list.h"
#include "../../trees/MerkleTree.h"

class Block {
public:
    Block() = default;
    Block(std::string hash, std::string previousHash, const std::string &merkleRoot,
          uint64_t time, uint64_t version, uint64_t transactionCount, uint64_t index, uint32_t size,
          int nonce, std::string message, std::string signP, std::string rewardP,
          const unit::list<ValidTransaction> &txList, uint64_t reward) : blockHeader(std::move(hash), std::move(previousHash), merkleRoot, time, version, transactionCount, index, size, nonce),
                                                                         message(std::move(message)), signP(std::move(signP)),
                                                                         rewardProverAddress(std::move(rewardP)), txList(txList),
                                                                         reward(reward) {}
    BlockHeader blockHeader;
    std::string message;
    std::string signP;
    std::string rP;
    std::string sP;
    std::string rewardProverAddress;
    unit::list<ValidTransaction> txList;
    uint64_t reward{};

    std::string serializeBlock();
    std::string serializeForSigning();
    std::string generateMerkleRoot();
    std::string generateHash();
    uint64_t getSize();
    const unit::list<ValidTransaction> *getTxList() const;

    const BlockHeader &getBlockHeader() const;

    void setBlockHeader(const BlockHeader &blockHeader);

    const std::string &getMessage() const;

    void setMessage(const std::string &message);

    const std::string &getSignP() const;

    void setSignP(const std::string &signP);

    const std::string &getRewardProverAddress() const;

    void setRewardProverAddress(const std::string &rewardProverAddress);

    void setTxList(const unit::list<ValidTransaction> &txList);

    uint64_t getReward() const;

    void setReward(uint64_t reward);

    const std::string &getRp() const;

    void setRp(const std::string &rP);

    const std::string &getSp() const;

    void setSp(const std::string &sP);
};

std::string Block::serializeBlock() {
    std::stringstream ss(blockHeader.serializeBlockHeader());
    ss << R"(", "message":")" << this->message << R"(", "signP":")" << this->signP << R"(", "rP":")" << this->rP << R"(", "sP":")" << this->sP << R"(", "rewardProverAddress":")"
       << this->rewardProverAddress << R"(", "reward":)" << this->reward << R"(})";
    return ss.str();
}

std::string Block::serializeForSigning() {
    std::stringstream ss(blockHeader.serializeBlockHeader());
    ss << R"(", "message":")" << this->message << R"(", "rewardProverAddress":")"
       << this->rewardProverAddress << R"(", "reward":)" << this->reward << R"(})";
    return ss.str();
}

std::string Block::generateMerkleRoot() {
    SHA3 sha3 = SHA3(SHA3::Bits256);
    if (txList.empty()) {
        std::vector<char> vec(32, '\0');
        std::string s(vec.begin(), vec.end());
        this->blockHeader.merkleRoot = sha3(sha3(s));
        return this->blockHeader.hash;
    }

    std::vector<std::string> leafs(this->txList.size());
    for (const ValidTransaction& tx : this->txList) leafs.emplace_back(tx.hash);
    MerkleTree merkleTree = MerkleTree(leafs);
    std::optional<std::string> root =  merkleTree.get_root();
    if (root->empty()) {
        std::vector<char> vec(32, '\0');
        std::string s(vec.begin(), vec.end());
        this->blockHeader.merkleRoot = sha3(sha3(s));
        return this->blockHeader.hash;
    }
    this->blockHeader.merkleRoot = root.value();
    return this->blockHeader.merkleRoot;
}

std::string Block::generateHash() {
    SHA3 sha3 = SHA3(SHA3::Bits256);
    this->blockHeader.hash = sha3(sha3(this->serializeBlock()));
    return this->blockHeader.hash;
}

uint64_t Block::getSize() {
    if(txList.empty()) return 0;
    for(auto & it : this->txList) this->blockHeader.size += it.getSize();
    return this->blockHeader.size;
}

const unit::list<ValidTransaction> *Block::getTxList() const {
    return &txList;
}

const BlockHeader &Block::getBlockHeader() const {
    return blockHeader;
}

void Block::setBlockHeader(const BlockHeader &blockHeader) {
    Block::blockHeader = blockHeader;
}

const std::string &Block::getMessage() const {
    return message;
}

void Block::setMessage(const std::string &message) {
    Block::message = message;
}

const std::string &Block::getSignP() const {
    return signP;
}

void Block::setSignP(const std::string &signP) {
    Block::signP = signP;
}

const std::string &Block::getRewardProverAddress() const {
    return rewardProverAddress;
}

void Block::setRewardProverAddress(const std::string &rewardProverAddress) {
    Block::rewardProverAddress = rewardProverAddress;
}

void Block::setTxList(const unit::list<ValidTransaction> &txList) {
    Block::txList = txList;
}

uint64_t Block::getReward() const {
    return reward;
}

void Block::setReward(uint64_t reward) {
    Block::reward = reward;
}

const std::string &Block::getRp() const {
    return rP;
}

void Block::setRp(const std::string &rP) {
    Block::rP = rP;
}

const std::string &Block::getSp() const {
    return sP;
}

void Block::setSp(const std::string &sP) {
    Block::sP = sP;
}

#endif //UNIT_BLOCK_H
