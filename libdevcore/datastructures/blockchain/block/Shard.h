//
// Created by Kirill Zhukov on 12.11.2022.
//

#ifndef UNIT_SHARD_H
#define UNIT_SHARD_H
#include "containers/list.h"
#include "transaction/Transaction.h"
#include "crypto/SHA3/sha3.h"
#include "trees/MerkleTree.h"

class Shard {
public:
    Shard() {}
    explicit Shard(const unit::vector<Transaction> &transactionList) : transactionList(transactionList) {}
    std::string transactionMerkleRoot;
    unit::vector<Transaction> transactionList;
    std::string shardID;
    std::string signature;
    std::string rP;
    std::string sP;

    const std::string &getTransactionMerkleRoot() const {
        return transactionMerkleRoot;
    }
    void setTransactionMerkleRoot() {
        std::vector<std::string> transactionsHash;
        transactionsHash.reserve(transactionList.size());
        for (auto &it : transactionList) if (!it.hash.empty()) transactionsHash.emplace_back(it.hash);
        MerkleTree merkleTree = MerkleTree(transactionsHash);
        this->transactionMerkleRoot = merkleTree.get_root();
    }

    const unit::vector<Transaction> &getTransactionList() const {
        return transactionList;
    }

    void setTransactionList(const unit::vector<Transaction> &transactionList) {
        Shard::transactionList = transactionList;
    }

    const std::string &getShardId() const {
        return shardID;
    }

    inline void insertList(const unit::vector<Transaction> &list) {
        this->transactionList.insert(this->transactionList.begin(), list.begin(), list.end());
    }

    Shard *setShardId(const std::string &address) {
        SHA3 sha3 = SHA3(SHA3::Bits256);
        shardID = sha3(sha3(address));
        return this;
    }

    void emplaceBack(Transaction &Transaction) {
        this->transactionList.emplace_back(Transaction);
    }

    std::string serializeTransactions() {
        std::stringstream ss;
        for (auto &it : this->transactionList)
            ss << R"(")" << it.hash << R"(",)";
        std::string serializedString = ss.str();
        serializedString.pop_back();
        return serializedString;
    }

    std::string serialize () {
        std::stringstream ss;
        ss << R"({"shardID":")" << this->shardID << R"(", "transactionMerkleRoot":")" << this->transactionMerkleRoot
        << R"(", "transactionList":[)" << this->serializeTransactions() << R"(])" << R"(, "signature":")" << this->signature
        << R"(", "rP":")" << this-> rP << R"(", "sP":")" << this-> sP << "\"}";
        return ss.str();
    }
    std::string serializeWithoutSignatures () {
        std::stringstream ss;
        ss << R"({"shardID":")" << this->shardID << R"(", "transactionMerkleRoot":")" << this->transactionMerkleRoot
        << R"(", "transactionList":[)" << this->serializeTransactions() << R"(]})";
        return ss.str();
    }

    const std::string &getSignature() const {
        return signature;
    }

    Shard *setSp(const std::string &sP) {
        this->sP = sP;
        return this;
    }
    Shard *setRp(const std::string &rP) {
        this->rP = rP;
        return this;
    }
    void setSignature(const std::string &signature) {
        this->signature = signature;
    }

};



#endif //UNIT_SHARD_H
