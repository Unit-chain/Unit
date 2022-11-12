//
// Created by Kirill Zhukov on 12.11.2022.
//

#ifndef UNIT_SHARD_H
#define UNIT_SHARD_H
#include "../../containers/list.h"
#include "../transaction/ValidTransaction.h"
#include "../../../crypto/SHA3/sha3.h"

class Shard {
public:
    std::string transactionMerkleRoot;
    unit::list<ValidTransaction> transactionList;
    std::string shardID;
    uint64_t size;

    const std::string &getTransactionMerkleRoot() const {
        return transactionMerkleRoot;
    }

    void setTransactionMerkleRoot(const std::string &transactionMerkleRoot) {
        Shard::transactionMerkleRoot = transactionMerkleRoot;
    }

    const unit::list<ValidTransaction> &getTransactionList() const {
        return transactionList;
    }

    void setTransactionList(const unit::list<ValidTransaction> &transactionList) {
        Shard::transactionList = transactionList;
    }

    const std::string &getShardId() const {
        return shardID;
    }

    void setShardId(const std::string &shardId) {
        shardID = shardId;
    }

    uint64_t getSize() const {
        return size;
    }

    void setSize(uint64_t size) {
        Shard::size = size;
    }

    void emplaceBack(ValidTransaction &validTransaction) {
        this->transactionList.push_back(validTransaction);
    }

    std::string serialize () {
        std::stringstream ss;
        ss << R"({)"
    }

};



#endif //UNIT_SHARD_H
