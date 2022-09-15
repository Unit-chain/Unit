//
// Created by Kirill Zhukov on 10.06.2022.
//

#ifndef UVM_TRANSACTION_H
#define UVM_TRANSACTION_H
#include "chrono"
#include "map"
#include "iostream"
#include <sstream>
#include "../Blockchain_core/Crypto/SHA3/sha3.h"
#include "boost/json.hpp"

class Transaction {
public:
    Transaction();

//    Transaction(const std::string &from, const std::string &to, uint64_t type,
//                std::map<std::string, std::string> &extraData, const std::string &previousHash, double amount);
//
//    Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date,
//                std::map<std::string, std::string> &extraData, const std::string &previousHash, double amount);
//
//    Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date,
//                std::map<std::string, std::string> &extraData, const std::string &hash,
//                const std::string &previousHash, double amount);

    Transaction(const std::string &from, const std::string &to, uint64_t type,
                const boost::json::value &extra, const std::string &previousHash, double amount);

    Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date, const boost::json::value &extra,
                const std::string &previousHash, double amount);

    Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date,
                const boost::json::value &extra, const std::string &hash, const std::string &previousHash,
                double amount);
    Transaction(const Transaction &tx);

    virtual ~Transaction();
    std::string from;
    std::string to;
    uint64_t type;
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    boost::json::value extra;
    std::string hash;
    uint64_t block_id;
    std::string sign;
    double amount;
    double fee; // in subunits; 1 unit = 100000 subunits;


    //  getters and setters
    [[nodiscard]] const std::string &getFrom() const;
    void setFrom(const std::string &from);
    [[nodiscard]] const std::string &getTo() const;
    void setTo(const std::string &to);
    [[nodiscard]] uint64_t getType() const;
    void setType(uint64_t type);
    [[nodiscard]] uint64_t getDate() const;
    void setDate(uint64_t date);
    [[nodiscard]] const std::map<std::string, std::string> &getExtraData() const;
    void setExtraData(const std::map<std::string, std::string> &extraData);
    [[nodiscard]] const std::string &getHash() const;
    void setHash(const std::string &hash);
    [[nodiscard]] const std::string &getPreviousHash() const;
    void setPreviousHash(const std::string &previousHash);
    [[nodiscard]] double getAmount() const;
    void setAmount(double amount);
    [[nodiscard]] double getFee() const;
    void setFee(double fee);
    [[nodiscard]] uint64_t getBlockId() const;
    void setBlockId(uint64_t blockId);
    [[nodiscard]] const std::string &getSign() const;
    void setSign(const std::string &sign);

    //  custom functions
    void generate_tx_hash(); // getting serializing value and make hashed it twice
    friend std::ostream& operator<< (std::ostream &out, const Transaction &transaction);
    std::string to_string();
    void set_current_date();
    std::string to_json_string();
    std::string to_json_string_test();

    //  boolean operators
    bool operator==(const Transaction &rhs) const;
    bool operator!=(const Transaction &rhs) const;

    bool operator<(const Transaction &rhs) const;

    bool operator>(const Transaction &rhs) const;

    bool operator<=(const Transaction &rhs) const;

    bool operator>=(const Transaction &rhs) const;
protected:
};


#endif //UVM_TRANSACTION_H