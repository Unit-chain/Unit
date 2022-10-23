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
    std::string rewardProverAddress;
    unit::list<ValidTransaction> txList;
    uint64_t reward{};

    std::string serializeBlock();
    std::string generateMerkleRoot();
    std::string generateHash();
};

std::string Block::serializeBlock() {
    std::stringstream ss(blockHeader.serializeBlockHeader());
    ss << R"(", "message":")" << this->message << R"(", "signP":")" << this->signP << R"(", "rewardProverAddress":")"
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

#endif //UNIT_BLOCK_H
