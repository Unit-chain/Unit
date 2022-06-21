//
// Created by Kirill Zhukov on 13.06.2022.
//

#include "Token.h"

const std::string &Token::getName() const {
    return name;
}

void Token::setName(const std::string &name) {
    Token::name = name;
}

const std::string &Token::getBytecode() const {
    return bytecode;
}

void Token::setBytecode(const std::string &bytecode) {
    Token::bytecode = bytecode;
}

double Token::getSupply() const {
    return supply;
}

void Token::setSupply(double supply) {
    Token::supply = supply;
}

std::ostream &operator<<(std::ostream &out, const Token &token) {
    std::string all_tx_to_string;
    for (std::string tx : token.transactions_in_token) {
        if (tx == token.transactions_in_token[token.transactions_in_token.size()-1]) {
            all_tx_to_string.append("\"").append(tx).append("\"");
            break;
        }
        all_tx_to_string.append("\"").append(tx).append("\", ");
    }
    return out << R"({"name":")"  << token.name << R"(", "bytecode":")" << token.bytecode << R"(", "token_hash":")" << token.token_hash << R"(", "supply":)" << std::fixed << token.supply << R"(, "date":)" << std::fixed << token.date << R"(, "transactions":)" <<  "[" << all_tx_to_string <<"]}";
}


std::string Token::to_json_string() const {
    std::ostringstream string_stream;
    string_stream << *this;
    return string_stream.str();
}

Token::~Token() {}

const std::string &Token::getTokenHash() const {
    return token_hash;
}

void Token::setTokenHash(const std::string &tokenHash) {
    token_hash = tokenHash;
}

void Token::generate_hash() {
    std::string json = this->to_json_without_txs();
    std::string first_hash = kec256::getHash(json, json.length());
    this->token_hash = kec256::getHash(first_hash, first_hash.length());
}

bool Token::operator==(const Token &rhs) const {
    return name == rhs.name &&
           bytecode == rhs.bytecode &&
           supply == rhs.supply &&
           date == rhs.date &&
           token_hash == rhs.token_hash;
}

bool Token::operator!=(const Token &rhs) const {
    return !(rhs == *this);
}

uint64_t Token::getDate() const {
    return date;
}

void Token::setDate(uint64_t date) {
    Token::date = date;
}

Token::Token(const std::string &name, const std::string &bytecode, const std::string &owner, double supply) : name(
        name), bytecode(bytecode), owner(owner), supply(supply) {
    this->name = name;
    this->bytecode = bytecode;
    this->owner = owner;
    this->supply = supply;
    this->generate_hash();
}

Token::Token(const std::string &name, const std::string &bytecode, const std::string &owner, double supply,
             std::vector<std::string> txs) {
    this->name = name;
    this->bytecode = bytecode;
    this->owner = owner;
    this->supply = supply;
    this->transactions_in_token = txs;
}

const std::string &Token::getOwner() const {
    return owner;
}

void Token::setOwner(const std::string &owner) {
    Token::owner = owner;
}

const std::vector<std::string> &Token::getTransactionsInToken() const {
    return transactions_in_token;
}

void Token::setTransactionsInToken(const std::vector<std::string> &transactionsInToken) {
    transactions_in_token = transactionsInToken;
}

std::string Token::to_json_without_txs() const {
    std::ostringstream stringstream;
    stringstream << R"({"name":")"  << this->name << R"(", "bytecode":")" << this->bytecode << R"(", "token_hash":")" << this->token_hash << R"(", "supply":)" << std::fixed << this->supply << R"(, "date":)" << std::fixed << this->date << "}";
    return stringstream.str();
}
