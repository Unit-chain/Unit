//
// Created by Kirill Zhukov on 13.11.2022.
//

#ifndef UNIT_BLOCK_H
#define UNIT_BLOCK_H

#include <utility>
#include "iostream"
#include "sstream"
#include "BlockHeader.h"
#include "Shard.h"
#include "transaction/Transaction.h"
#include "containers/list.h"
#include "containers/vector.h"
#include "trees/MerkleTree.h"

class Block {
public:
    Block() = default;
    Block(std::string hash, std::string previousHash, const std::string &merkleRoot,
             uint64_t time, uint64_t version, uint64_t transactionCount, uint64_t index, uint32_t size,
             int nonce, std::string message, std::string signP, std::string rewardP,
             const unit::vector<Shard> &shardList, std::string &reward) : blockHeader(std::move(hash), std::move(previousHash), merkleRoot, time, version, transactionCount, index, size, nonce),
                                                                    message(std::move(message)), signP(std::move(signP)),
                                                                    rewardProverAddress(std::move(rewardP)), shardList(shardList),
                                                                    reward(reward) {}
    BlockHeader blockHeader;
    std::string message;
    std::string signP;
    std::string rP;
    std::string sP;
    std::string difficulty;
    std::string miner = "LOCAL_TEST";
    std::string rewardProverAddress;
    std::string logsBloom = "0x0";
    unit::vector<Shard> shardList{};
    std::string reward{};
    std::string epoch{};

    std::string serializeBlock();
    std::string serializeForSigning();
    std::string generateMerkleRoot();
    std::string generateHash();
    uint64_t getSize();
    const unit::vector<Shard> &getShardList() const;

    const BlockHeader &getBlockHeader() const;

    void setBlockHeader(const BlockHeader &blockHeader);

    const std::string &getMessage() const;

    void setMessage(const std::string &message);

    const std::string &getSignP() const;

    void setSignP(const std::string &signP);

    const std::string &getRewardProverAddress() const;

    void setRewardProverAddress(const std::string &rewardProverAddress);

    void setTxList(const unit::vector<Transaction> &txList);

    std::string getReward() const;

    void setReward(std::string &reward);

    const std::string &getRp() const;

    void setRp(const std::string &rP);

    const std::string &getSp() const;

    void setSp(const std::string &sP);

    std::string getEpoch() const;

    void setEpoch(std::string &epoch);

    void setShardList(const unit::vector<Shard> &txList);

    const std::string &getDifficulty() const;

    void setDifficulty(const std::string &difficulty);

    const std::string &getMiner() const;

    void setMiner(const std::string &miner);

    const std::string &getLogsBloom() const;

    void setLogsBloom(const std::string &logsBloom);

    void emplaceBack(Shard &shard);

    std::string serializeShards();

    Block &operator=(const Block& c2) {
        this->blockHeader = c2.blockHeader;
        this->message = c2.message;
        this->signP = c2.signP;
        this->rP = c2.rP;
        this->sP = c2.sP;
        this->difficulty = c2.difficulty;
        this->miner = c2.miner;
        this->rewardProverAddress = c2.rewardProverAddress;
        this->logsBloom = c2.logsBloom;
        this->shardList = c2.shardList;
        this->reward = c2.reward;
        this->epoch = c2.epoch;
    }
};

std::string Block::serializeShards() {
    if (this->shardList.empty()) return "";
    std::stringstream ss;
    for (auto &shard : this->shardList) ss << shard.serialize() << ",";
    std::string shard = ss.str();
    shard.pop_back();
    return shard;
}

std::string Block::serializeBlock() {
    std::stringstream ss;
    ss << blockHeader.serializeBlockHeader() << R"(, "message":")" << this->message << R"(", "signP":")" << this->signP << R"(", "rP":")" << this->rP << R"(", "sP":")" << this->sP << R"(", "rewardProverAddress":")"
       << this->rewardProverAddress << R"(", "logsBloom": ")" << this->logsBloom << R"(", "difficulty":")" << this->difficulty << R"(", "miner":")" << this->miner
       << R"(", "reward":")" << this->reward << R"(", "shards":[)" << this->serializeShards() << R"(]})";
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

std::string Block::generateHash() {
    SHA3 sha3 = SHA3(SHA3::Bits256);
    this->blockHeader.hash = sha3(sha3(this->serializeBlock()));
    return this->blockHeader.hash;
}

uint64_t Block::getSize() {
    if(shardList.empty()) return 0;
    for(auto & it : this->shardList) this->blockHeader.size += it.serialize().size();
    return this->blockHeader.size;
}

const unit::vector<Shard> &Block::getShardList() const {
    return shardList;
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
    this->message = message;
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

void Block::setShardList(const unit::vector<Shard> &txList) {
    Block::shardList = txList;
}

std::string Block::getReward() const {
    return reward;
}

void Block::setReward(std::string &reward) {
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

std::string Block::getEpoch() const {
    return epoch;
}

void Block::setEpoch(std::string &epoch) {
    Block::epoch = epoch;
}

const std::string &Block::getDifficulty() const {
    return difficulty;
}

void Block::setDifficulty(const std::string &difficulty) {
    Block::difficulty = difficulty;
}

const std::string &Block::getMiner() const {
    return miner;
}

void Block::setMiner(const std::string &miner) {
    Block::miner = miner;
}

const std::string &Block::getLogsBloom() const {
    return logsBloom;
}

void Block::setLogsBloom(const std::string &logsBloom) {
    Block::logsBloom = logsBloom;
}

void Block::emplaceBack(Shard &shard) {
    this->shardList.emplace_back(shard);
}

#endif //UNIT_BLOCK_H
