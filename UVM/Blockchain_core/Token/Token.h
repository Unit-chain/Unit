//
// Created by Kirill Zhukov on 13.06.2022.
//

#ifndef UVM_TOKEN_H
#define UVM_TOKEN_H
#include "iostream"
#include "string"
#include "vector"
#include "chrono"
#include "../Transaction.h"
#include "../Crypto/Keccak/kec256.h"

class Token {
public:
    Token(const std::string &name, const std::string &bytecode, const std::string &owner, double supply);
    Token(const std::string &name, const std::string &bytecode, const std::string &owner, double supply, std::vector<std::string> txs);
    virtual ~Token();

    std::string name;
    std::string bytecode;
    std::string owner;
    std::vector<std::string> transactions_in_token;
    double supply;
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::string token_hash;

    [[nodiscard]] const std::string &getName() const;
    void setName(const std::string &name);
    [[nodiscard]] const std::string &getBytecode() const;
    void setBytecode(const std::string &bytecode);
    [[nodiscard]] double getSupply() const;
    void setSupply(double supply);
    [[nodiscard]] const std::string &getTokenHash() const;
    void setTokenHash(const std::string &tokenHash);
    [[nodiscard]] uint64_t getDate() const;
    void setDate(uint64_t date);
    [[nodiscard]] const std::string &getOwner() const;
    void setOwner(const std::string &owner);
    [[nodiscard]] const std::vector<std::string> &getTransactionsInToken() const;

    void setTransactionsInToken(const std::vector<std::string> &transactionsInToken);

    friend std::ostream& operator<< (std::ostream &out, const Token &token);

    bool operator==(const Token &rhs) const;

    bool operator!=(const Token &rhs) const;

    [[nodiscard]] std::string to_json_string() const;
    [[nodiscard]] std::string to_json_without_txs() const;
    Token prepare_token(Transaction &transaction);
    void generate_hash();
};


#endif //UVM_TOKEN_H
