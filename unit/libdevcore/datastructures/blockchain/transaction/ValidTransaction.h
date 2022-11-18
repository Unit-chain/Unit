//
// Created by Kirill Zhukov on 21.10.2022.
//

#ifndef UNIT_VALIDTRANSACTION_H
#define UNIT_VALIDTRANSACTION_H

#include "utility"
#include "iostream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "../../request/RawTransaction.h"

namespace json = boost::json;
using namespace boost::multiprecision;

class ValidTransaction {
public:
    ValidTransaction() = default;
    explicit ValidTransaction(RawTransaction *rawPointer) {
        this->to = rawPointer->to;
        this->from = rawPointer->from;
        this->type = rawPointer->type;
        this->date = rawPointer->date;
        this->extra = rawPointer->extra;
        this->hash = rawPointer->hash;
        this->sign = rawPointer->sign;
        this->r = rawPointer->r;
        this->s = rawPointer->s;
        this->signP = rawPointer->signP;
        this->rP = rawPointer->rP;
        this->sP = rawPointer->sP;
        this->amount = rawPointer->amount;
        this->fee = rawPointer->fee;
        this->nonce = rawPointer->nonce;
    }
    explicit ValidTransaction(RawTransaction &rawTransaction) {
        this->to = rawTransaction.to;
        this->from = rawTransaction.from;
        this->type = rawTransaction.type;
        this->date = rawTransaction.date;
        this->extra = rawTransaction.extra;
        this->hash = rawTransaction.hash;
        this->sign = rawTransaction.sign;
        this->r = rawTransaction.r;
        this->s = rawTransaction.s;
        this->signP = rawTransaction.signP;
        this->rP = rawTransaction.rP;
        this->sP = rawTransaction.sP;
        this->amount = rawTransaction.amount;
        this->fee = rawTransaction.fee;
        this->nonce = rawTransaction.nonce;
    }
    explicit ValidTransaction(RawTransaction &&rawTransaction) {
        this->to = rawTransaction.to;
        this->from = rawTransaction.from;
        this->type = rawTransaction.type;
        this->date = rawTransaction.date;
        this->extra = rawTransaction.extra;
        this->hash = rawTransaction.hash;
        this->sign = rawTransaction.sign;
        this->r = rawTransaction.r;
        this->s = rawTransaction.s;
        this->signP = rawTransaction.signP;
        this->rP = rawTransaction.rP;
        this->sP = rawTransaction.sP;
        this->amount = rawTransaction.amount;
        this->fee = rawTransaction.fee;
        this->nonce = rawTransaction.nonce;
    }
    std::string from;
    std::string to;
    uint64_t type{};
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    json::value extra;
    std::string hash;
    std::string sign;
    std::string r;
    std::string s;
    std::string signP; // prover signature
    std::string rP; // prover signature
    std::string sP; // prover signature
    uint32_t index;
    uint64_t nonce;
    uint256_t amount{};
    uint64_t fee{};

    inline std::shared_ptr<std::string> getHash() {
        return std::make_shared<std::string>(this->hash);
    }

    [[nodiscard]] inline std::string serializeToJsonTransaction() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << ValidTransaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s << R"("})";
        }
        return ss.str();
    }

    [[nodiscard]] inline uint32_t getSize() const {
        return static_cast<uint32_t>((this->serializeToJsonTransaction()).size());
    }

    void setIndex(uint32_t index) {
        ValidTransaction::index = index;
    }

    [[deprecated("Not implemented yet")]]
    void setZKProveData() {}

    bool operator==(const ValidTransaction &rhs) const {
        return from == rhs.from &&
               to == rhs.to &&
               type == rhs.type &&
               date == rhs.date &&
               extra == rhs.extra &&
               hash == rhs.hash &&
               sign == rhs.sign &&
               r == rhs.r &&
               s == rhs.s &&
               signP == rhs.signP &&
               rP == rhs.rP &&
               sP == rhs.sP &&
               index == rhs.index &&
               nonce == rhs.nonce &&
               amount == rhs.amount &&
               fee == rhs.fee;
    }

    bool operator!=(const ValidTransaction &rhs) const {
        return !(rhs == *this);
    }
protected:
    inline static std::string uint256_jv_2string(const boost::json::value &value) {
        return boost::json::value_to<std::string>(value);
    }
};


#endif //UNIT_VALIDTRANSACTION_H
