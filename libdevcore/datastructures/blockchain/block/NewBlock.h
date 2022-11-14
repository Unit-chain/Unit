//
// Created by Kirill Zhukov on 13.11.2022.
//

#ifndef UNIT_NEWBLOCK_H
#define UNIT_NEWBLOCK_H

#include <utility>
#include "iostream"
#include "sstream"
#include "BlockHeader.h"
#include "Shard.h"
#include "../transaction/ValidTransaction.h"
#include "../../containers/list.h"
#include "../../trees/MerkleTree.h"

class NewBlock {
public:
    NewBlock() = default;
    NewBlock(std::string hash, std::string previousHash, const std::string &merkleRoot,
             uint64_t time, uint64_t version, uint64_t transactionCount, uint64_t index, uint32_t size,
             int nonce, std::string message, std::string signP, std::string rewardP,
             const unit::list<Shard> &shardList, uint64_t reward) : blockHeader(std::move(hash), std::move(previousHash), merkleRoot, time, version, transactionCount, index, size, nonce),
                                                                    message(std::move(message)), signP(std::move(signP)),
                                                                    rewardProverAddress(std::move(rewardP)), shardList(shardList),
                                                                    reward(reward) {}
    BlockHeader blockHeader;
    std::string message;
    std::string signP;
    std::string rP;
    std::string sP;
    std::string difficulty;
    std::string miner;
    std::string rewardProverAddress;
    std::string logsBloom = "0x0";
    unit::list<Shard> shardList{};
    uint64_t reward{};
    uint64_t epoch{};

    std::string serializeBlock();
    std::string serializeForSigning();
    std::string generateMerkleRoot();
    std::string generateHash();
    uint64_t getSize();
    const unit::list<Shard> &getShardList() const;

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

    uint64_t getEpoch() const;

    void setEpoch(uint64_t epoch);

    void setShardList(const unit::list<Shard> &txList);

    const std::string &getDifficulty() const;

    void setDifficulty(const std::string &difficulty);

    const std::string &getMiner() const;

    void setMiner(const std::string &miner);

    const std::string &getLogsBloom() const;

    void setLogsBloom(const std::string &logsBloom);

    void emplaceBack(const ValidTransaction &validTransaction);

    std::string serializeShards();
};

std::string NewBlock::serializeShards() {
    std::stringstream ss;
    for (auto &shard : this->shardList) ss << shard.serialize() << ",";
    std::string shard = ss.str();
    shard.pop_back();
    return shard;
}

std::string NewBlock::serializeBlock() {
    std::stringstream ss(blockHeader.serializeBlockHeader());
    ss << R"(", "message":")" << this->message << R"(", "signP":")" << this->signP << R"(", "rP":")" << this->rP << R"(", "sP":")" << this->sP << R"(", "rewardProverAddress":")"
       << this->rewardProverAddress << R"(", "logsBloom": ")" << this->logsBloom << R"(", "difficulty":")" << this->difficulty << R"(", "miner":")" << this->miner
       << R"(", "miner":")" << this->miner << R"(", "reward":")" << this->reward << R"(", "shards":[)" << this->serializeShards() << R"(]})";
    return ss.str();
}

std::string NewBlock::serializeForSigning() {
    std::stringstream ss(blockHeader.serializeBlockHeader());
    ss << R"(", "message":")" << this->message << R"(", "rewardProverAddress":")"
       << this->rewardProverAddress << R"(", "reward":)" << this->reward << R"(})";
    return ss.str();
}

std::string NewBlock::generateMerkleRoot() {
    SHA3 sha3 = SHA3(SHA3::Bits256);
    if (shardList.empty()) {
        std::vector<char> vec(32, '\0');
        std::string s(vec.begin(), vec.end());
        this->blockHeader.merkleRoot = sha3(sha3(s));
        return this->blockHeader.hash;
    }

    std::vector<std::string> leafs(this->shardList.size());
    for (const Shard &shard : this->shardList) leafs.emplace_back(shard.transactionMerkleRoot);
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

std::string NewBlock::generateHash() {
    SHA3 sha3 = SHA3(SHA3::Bits256);
    this->blockHeader.hash = sha3(sha3(this->serializeBlock()));
    return this->blockHeader.hash;
}

uint64_t NewBlock::getSize() {
    if(shardList.empty()) return 0;
    for(auto & it : this->shardList) this->blockHeader.size += it.getSize();
    return this->blockHeader.size;
}

const unit::list<Shard> &NewBlock::getShardList() const {
    return shardList;
}

const BlockHeader &NewBlock::getBlockHeader() const {
    return blockHeader;
}

void NewBlock::setBlockHeader(const BlockHeader &blockHeader) {
    NewBlock::blockHeader = blockHeader;
}

const std::string &NewBlock::getMessage() const {
    return message;
}

void NewBlock::setMessage(const std::string &message) {
    NewBlock::message = message;
}

const std::string &NewBlock::getSignP() const {
    return signP;
}

void NewBlock::setSignP(const std::string &signP) {
    NewBlock::signP = signP;
}

const std::string &NewBlock::getRewardProverAddress() const {
    return rewardProverAddress;
}

void NewBlock::setRewardProverAddress(const std::string &rewardProverAddress) {
    NewBlock::rewardProverAddress = rewardProverAddress;
}

void NewBlock::setShardList(const unit::list<Shard> &txList) {
    NewBlock::shardList = txList;
}

uint64_t NewBlock::getReward() const {
    return reward;
}

void NewBlock::setReward(uint64_t reward) {
    NewBlock::reward = reward;
}

const std::string &NewBlock::getRp() const {
    return rP;
}

void NewBlock::setRp(const std::string &rP) {
    NewBlock::rP = rP;
}

const std::string &NewBlock::getSp() const {
    return sP;
}

void NewBlock::setSp(const std::string &sP) {
    NewBlock::sP = sP;
}

uint64_t NewBlock::getEpoch() const {
    return epoch;
}

void NewBlock::setEpoch(uint64_t epoch) {
    NewBlock::epoch = epoch;
}

const std::string &NewBlock::getDifficulty() const {
    return difficulty;
}

void NewBlock::setDifficulty(const std::string &difficulty) {
    NewBlock::difficulty = difficulty;
}

const std::string &NewBlock::getMiner() const {
    return miner;
}

void NewBlock::setMiner(const std::string &miner) {
    NewBlock::miner = miner;
}

const std::string &NewBlock::getLogsBloom() const {
    return logsBloom;
}

void NewBlock::setLogsBloom(const std::string &logsBloom) {
    NewBlock::logsBloom = logsBloom;
}

#endif //UNIT_NEWBLOCK_H
