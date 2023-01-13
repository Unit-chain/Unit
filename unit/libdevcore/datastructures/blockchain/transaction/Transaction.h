//
// Created by Kirill Zhukov on 20.10.2022.
//

#ifndef UNIT_TRANSACTION_H
#define UNIT_TRANSACTION_H

#include <utility>

#include "utility"
#include "sstream"
#include "iostream"
#include "optional"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "utils/StringUtil.h"
#include "crypto/SHA3/sha3.h"

namespace json = boost::json;
using namespace boost::multiprecision;

std::string hexView(const uint256_t& value) {
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}

class Transaction {
public:
    virtual ~Transaction() = default;
    Transaction() = default;

    Transaction(std::string from,
                std::string to,
                uint64_t type,
                json::value extra,
                std::string sign,
                std::string r,
                std::string s,
                uint256_t amount,
                uint64_t fee,
                uint64_t nonce) :
            from(std::move(from)),
            to(std::move(to)),
            type(type),
            extra(std::move(extra)),
            sign(std::move(sign)),
            r(std::move(r)),
            s(std::move(s)),
            amount(std::move(amount)),
            fee(fee),
            nonce(nonce) {}

    Transaction(std::string from,
                std::string to,
                uint64_t type,
                std::string sign,
                std::string r,
                std::string s,
                uint256_t amount,
                uint64_t fee,
                uint64_t nonce) :
            from(std::move(from)),
            to(std::move(to)),
            type(type),
            sign(std::move(sign)),
            r(std::move(r)),
            s(std::move(s)),
            amount(std::move(amount)),
            fee(fee),
            nonce(nonce) {}

    std::string from;
    std::string to;
    uint32_t type{};
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    json::value extra;
    std::string hash;
    std::string sign;
    std::string r;
    std::string s;
    std::string signP; // prover signature
    std::string rP; // prover signature
    std::string sP; // prover signature
    uint64_t nonce;
    uint256_t amount{};
    uint64_t fee{};
    bool pending = false;

    inline void setProverData(std::string &signP, std::string &rP, std::string &sP) {
        this->signP = signP;
        this->signP = rP;
        this->signP = sP;
    }

    inline std::shared_ptr<std::string> getHash() {
        return std::make_shared<std::string>(this->hash);
    }

    inline void generateHash() {
        SHA3 sha3 = SHA3(SHA3::Bits256);
        this->hash = sha3(sha3(this->serializeToRawTransaction()));
    }

    [[nodiscard]] inline std::string serializeWithoutSignatures() const {
        std::stringstream ss;
        if (type == 0) {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "type":)"
               << this->type << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "amount":")" << "0x" << hexView(this->amount) << R"("})";
        } else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":)" << Transaction::uint256_jv_2string(this->extra.at("value")) << R"(, "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)" << this->nonce << R"(})";
        }
        return ss.str();
    }

    [[nodiscard]] inline std::string serializeForResponse() const {
        std::stringstream ss;
        if (type == 0) {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "type":)"
               << this->type << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "amount":")" << "0x" << hexView(this->amount) << R"(", "pending":)" << this->pending << R"(})";
        } else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":)" << Transaction::uint256_jv_2string(this->extra.at("value")) << R"(, "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)" << this->nonce << R"(})";
        }
        return ss.str();
    }

    [[nodiscard]] inline std::string serializeForSending() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << Transaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return ss.str();
    }

    [[nodiscard]] inline std::string serializeToRawTransaction() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << Transaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return ss.str();
    }

    [[nodiscard]] inline std::string serializeToJsonTransaction() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(", "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":")" << "0x"
               << hexView(this->amount) << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << Transaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return ss.str();
    }

    static Transaction parse(boost::json::value *params) {
        boost::json::object transactionRequestJson = params->as_object();
        transactionRequestJson = transactionRequestJson["params"].as_object();
        auto type = boost::json::value_to<uint64_t>(transactionRequestJson.at("type"));
        if (type == 0)
            return {
                    boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("signature")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                    uint256_t(boost::json::value_to<std::string>(transactionRequestJson.at("amount"))),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("fee")),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("nonce"))};
        return {
                boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                transactionRequestJson.at("extradata"),
                boost::json::value_to<std::string>(transactionRequestJson.at("signature")),
                boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                uint256_t(boost::json::value_to<std::string>(transactionRequestJson.at("amount"))),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("fee")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("nonce"))};
    }

    [[maybe_unused]] static Transaction parseToGenesis(const std::string& serializedTx) {
        boost::json::value transactionRequestJson = boost::json::parse(serializedTx);
        return {boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                boost::json::value_to<std::string>(transactionRequestJson.at("signature")),
                boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                uint256_t(boost::json::value_to<std::string>(transactionRequestJson.at("amount"))),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("fee")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("nonce"))};
    }

    bool operator==(const Transaction &rhs) const {
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
               nonce == rhs.nonce &&
               amount == rhs.amount &&
               fee == rhs.fee &&
               pending == rhs.pending;
    }

    bool operator!=(const Transaction &rhs) const {
        return !(rhs == *this);
    }

protected:
    inline static std::string uint256_jv_2string(const boost::json::value &value) {
        return boost::json::value_to<std::string>(value);
    }
};
#endif //UNIT_TRANSACTION_H
