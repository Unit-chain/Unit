//
// Created by Kirill Zhukov on 10.06.2022.
//

#include <string>
#include <map>
#include "Transaction.h"
#include "format"
#include "Crypto/kec256.h"

Transaction::Transaction() {}

Transaction::~Transaction() {}

const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &
Transaction::getFrom() const {
    return from;
}

void
Transaction::setFrom(const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &from) {
    Transaction::from = from;
}

const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &Transaction::getTo() const {
    return to;
}

void
Transaction::setTo(const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &to) {
    Transaction::to = to;
}

uint64_t Transaction::getType() const {
    return type;
}

void Transaction::setType(uint64_t type) {
    Transaction::type = type;
}

uint64_t Transaction::getDate() const {
    return date;
}

void Transaction::setDate(uint64_t date) {
    Transaction::date = date;
}

const std::__1::map<std::string, std::string> &Transaction::getExtraData() const {
    return extra_data;
}

void Transaction::setExtraData(const std::__1::map<std::string, std::string> &extraData) {
    extra_data = extraData;
}

void Transaction::generate_tx_hash() {
    std::string tx_as_str = this->to_string();
    std::string tx_hash = kec256::getHash(tx_as_str, tx_as_str.length());
    this->hash = kec256::getHash(tx_hash, tx_hash.length());
}

bool Transaction::operator==(const Transaction &rhs) const {
    return from == rhs.from &&
           to == rhs.to &&
           type == rhs.type &&
           date == rhs.date &&
           extra_data == rhs.extra_data &&
           hash == rhs.hash &&
           previous_hash == rhs.previous_hash;
}

bool Transaction::operator!=(const Transaction &rhs) const {
    return !(rhs == *this);
}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type,
                         const std::map<std::string, std::string> &extraData, const std::string &previousHash,
                         uint64_t date) {
    this->from = from;
    this->to = to;
    this->type = type;
    this->extra_data = extraData;
    this->previous_hash = previousHash;
    this->date = date;
//    std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type,
                         const std::map<std::string, std::string> &extraData, const std::string &hash,
                         const std::string &previousHash, uint64_t date) {
    this->from = from;
    this->to = to;
    this->type = type;
    this->extra_data = extraData;
    this->hash = hash;
    this->previous_hash = previousHash;
    this->date = date;
}


std::ostream &operator<<(std::ostream &out, const Transaction &transaction) {
    std::map<std::string, std::string> extra_map = transaction.extra_data;
    return out << "Transaction{" << transaction.from << ", " << transaction.to << ", " << transaction.type << ", " << transaction.date << ", " << extra_map["name"] << ", " << extra_map["value"] << ", " << transaction.previous_hash <<  "}";
}

std::string Transaction::to_string() {
    std::ostringstream string_stream;
    string_stream << *this;
    return string_stream.str();
}

void Transaction::set_current_date() {
    this->date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type,
                         const std::map<std::string, std::string> &extraData, const std::string &hash,
                         const std::string &previousHash) : from(from), to(to), type(type), extra_data(extraData),
                                                            hash(hash), previous_hash(previousHash) {
    this->from = from;
    this->to = to;
    this->type = type;
    this->extra_data = extraData;
    this->hash = hash;
    this->previous_hash = previousHash;
}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type,
                         const std::map<std::string, std::string> &extraData, const std::string &previousHash) : from(
        from), to(to), type(type), extra_data(extraData), previous_hash(previousHash) {
    this->from = from;
    this->to = to;
    this->type = type;
    this->extra_data = extraData;
    this->previous_hash = previousHash;
}
