//
// Created by Kirill Zhukov on 10.06.2022.
//

#ifndef UVM_BLOCK_H
#define UVM_BLOCK_H
#include "iostream"
#include "chrono"
#include "vector"
#include "Transaction.h"
#include "rocksdb/db.h"

class Block {
public:
    Block(){}

    virtual ~Block();

    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t index;
    uint16_t net_version;
    std::string hash;
    std::string prev_hash;
//    std::vector<Transaction> transactions;

    std::string previous_block_num();

    [[nodiscard]] uint64_t getDate() const {
        return date;
    }

    void setDate(uint64_t date) {
        Block::date = date;
    }

    [[nodiscard]] uint64_t getIndex() const {
        return index;
    }

    void setIndex(uint64_t index) {
        Block::index = index;
    }

    [[nodiscard]] uint16_t getNetVersion() const {
        return net_version;
    }

    void setNetVersion(uint16_t netVersion) {
        net_version = netVersion;
    }

    [[nodiscard]] const std::string &getHash() const {
        return hash;
    }

    void setHash(const std::string &hash) {
        Block::hash = hash;
    }

    [[nodiscard]] const std::string &getPrevHash() const {
        return prev_hash;
    }

    void setPrevHash(const std::string &prevHash) {
        prev_hash = prevHash;
    }

//    [[nodiscard]] const std::vector<Transaction> &getTransactions() const {
//        return transactions;
//    }
//
//    void setTransactions(const std::vector<Transaction> &transactions) {
//        Block::transactions = transactions;
//    }

    friend std::ostream& operator<< (std::ostream &out, const Block &block);
};
#endif //UVM_BLOCK_H
