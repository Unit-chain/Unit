//
// Created by Kirill Zhukov on 10.06.2022.
//

#ifndef UVM_BLOCK_H
#define UVM_BLOCK_H
#include "iostream"
#include "chrono"
#include "vector"
#include "sstream"
#include "Transaction.h"
#include "rocksdb/db.h"
#include "Crypto/kec256.h"
//#include "../ENV/env.h"

class Block {
public:
    Block(){}

    virtual ~Block();

    Block(uint64_t index, uint16_t netVersion, const std::string &prevHash,
          const std::vector<Transaction> &transactions);
    Block(uint64_t index, uint16_t netVersion, const std::string &prevHash);
    Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &prevHash);
    Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &prevHash,
          const std::vector<Transaction> &transactions);

    Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &hash, const std::string &prevHash,
          const std::vector<Transaction> &transactions);

    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t index;
    uint16_t net_version;
    std::string hash;
    std::string prev_hash;
    std::vector<Transaction> transactions;

    std::string previous_block_num();
    void generate_hash();
    void push_tx(Transaction &tx);
    void set_current_date();

    [[nodiscard]] uint64_t getDate() const;
    void setDate(uint64_t date);
    [[nodiscard]] uint64_t getIndex() const;
    void setIndex(uint64_t index);
    [[nodiscard]] uint16_t getNetVersion() const;
    void setNetVersion(uint16_t netVersion);
    [[nodiscard]] const std::string &getHash() const;
    void setHash(const std::string &hash);
    [[nodiscard]] const std::string &getPrevHash() const;
    void setPrevHash(const std::string &prevHash);
    [[nodiscard]] const std::vector<Transaction> &getTransactions() const;
    void setTransactions(const std::vector<Transaction> &transactions);
    friend std::ostream& operator<< (std::ostream &out, const Block &block);
    std::string to_string();
};
#endif //UVM_BLOCK_H
