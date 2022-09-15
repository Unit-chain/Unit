//
// Created by Kirill Zhukov on 10.06.2022.
//

#include <string>
#include "Transaction.h"
#include "Crypto/Keccak/kec256.h"

Transaction::Transaction() {}

Transaction::~Transaction() {}

uint64_t Transaction::getType() const {
    return this->type;
}

void Transaction::setType(uint64_t type) {
    this->type = type;
}

uint64_t Transaction::getDate() const {
    return this->date;
}

void Transaction::setDate(uint64_t date) {
    this->date = date;
}

void Transaction::generate_tx_hash() {
    std::string tx_as_str = this->to_json_string_test();
    SHA3 sha3 = SHA3(SHA3::Bits256);
    std::string tx_hash = sha3(tx_as_str);
    this->hash = "0x"+sha3(tx_hash);
}

std::ostream &operator<<(std::ostream &out, const Transaction &transaction) {
    transaction.extra;
    return out << "{\"" << transaction.from << "\", \"" << transaction.to << "\", " << transaction.type << ", " << transaction.date << ", \"" << transaction.extra.at("name") << "\", \"" << transaction.extra.at("value") << "\", " << transaction.amount <<  "}";
}

std::string Transaction::to_string() {
    std::ostringstream string_stream;
    string_stream << *this;
    return string_stream.str();
}

void Transaction::set_current_date() {
    this->date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

const std::string &Transaction::getHash() const {
    return this->hash;
}

void Transaction::setHash(const std::string &hash) {
    this->hash = hash;
}

double Transaction::getAmount() const {
    return this->amount;
}

void Transaction::setAmount(double amount) {
    this->amount = amount;
}

bool Transaction::operator==(const Transaction &rhs) const {
    return from == rhs.from &&
            this->to == rhs.to &&
            this->type == rhs.type &&
            this->date == rhs.date &&
            this->extra == rhs.extra &&
            this->hash == rhs.hash &&
            this->amount == rhs.amount;
}

bool Transaction::operator!=(const Transaction &rhs) const {
    return !(rhs == *this);
}

double Transaction::getFee() const {
    return this->fee;
}

void Transaction::setFee(double fee) {
    this->fee = fee;
}

std::string Transaction::to_json_string() {
    std::ostringstream string_stream;
    string_stream << R"({"hash":")" << this->hash << R"(", "from":")" << this->from << R"(", "to":")" << this->to << R"(", "type":)" << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << this->extra.at("name") << R"(", "value":")" << this->extra.at("value") << R"(", "bytecode":")" << this->extra.at("bytecode") << "\"}" << R"(, "sign":")" << this->sign << R"(", "block_id":")" << this->block_id << R"(", "amount":)"  << std::fixed << this->amount <<  "}";
    return string_stream.str();
}

const std::string &Transaction::getFrom() const {
    return this->from;
}

void Transaction::setFrom(const std::string &from) {
    this->from = from;
}

const std::string &Transaction::getTo() const {
    return this->to;
}

void Transaction::setTo(const std::string &to) {
    this->to = to;
}

uint64_t Transaction::getBlockId() const {
    return block_id;
}

void Transaction::setBlockId(uint64_t blockId) {
    block_id = blockId;
}

const std::string &Transaction::getSign() const {
    return sign;
}

void Transaction::setSign(const std::string &sign) {
    Transaction::sign = sign;
}

bool Transaction::operator<(const Transaction &rhs) const {
    if (from < rhs.from)
        return true;
    if (rhs.from < from)
        return false;
    if (to < rhs.to)
        return true;
    if (rhs.to < to)
        return false;
    if (type < rhs.type)
        return true;
    if (rhs.type < type)
        return false;
    if (date < rhs.date)
        return true;
    if (rhs.date < date)
        return false;
    if (hash < rhs.hash)
        return true;
    if (rhs.hash < hash)
        return false;
    if (block_id < rhs.block_id)
        return true;
    if (rhs.block_id < block_id)
        return false;
    if (sign < rhs.sign)
        return true;
    if (rhs.sign < sign)
        return false;
    if (amount < rhs.amount)
        return true;
    if (rhs.amount < amount)
        return false;
    return fee < rhs.fee;
}

bool Transaction::operator>(const Transaction &rhs) const {
    return rhs < *this;
}

bool Transaction::operator<=(const Transaction &rhs) const {
    return !(rhs < *this);
}

bool Transaction::operator>=(const Transaction &rhs) const {
    return !(*this < rhs);
}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type,
                         const boost::json::value &extra, const std::string &previousHash,
                         double amount) : from(from), to(to), type(type), extra(extra), amount(amount) {}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date, const boost::json::value &extra,
                         const std::string &previousHash, double amount) : from(from), to(to), type(type), date(date), extra(extra),
                                                                           amount(amount) {}

Transaction::Transaction(const std::string &from, const std::string &to, uint64_t type, uint64_t date,
                         const boost::json::value &extra, const std::string &hash, const std::string &previousHash,
                         double amount) : from(from), to(to), type(type), date(date), extra(extra), hash(hash), amount(amount) {}

std::string Transaction::to_json_string_test() {
    std::ostringstream string_stream;
    string_stream << R"({"hash":")" << this->hash << R"(", "from":")" << this->from << R"(", "to":")" << this->to << R"(", "type":)" << this->type << R"(, "date":)" << this->date << R"(, "extradata":)" << extra << R"(, "sign":")" << this->sign << R"(", "amount":)"  << std::fixed << this->amount <<  "}";
    return string_stream.str();
}

Transaction::Transaction(const Transaction &tx) {
    this->from = tx.from;
    this->to = tx.to;
    this->date = tx.date;
    this->amount = tx.amount;
    this->type = tx.type;
    this->hash = tx.hash;
    std::cout << "tx#" << tx.hash << " extra: " << tx.extra << std::endl;
    this->extra = tx.extra;
    this->block_id = tx.block_id;
    this->sign = tx.sign;
    this->fee = tx.fee;
}
