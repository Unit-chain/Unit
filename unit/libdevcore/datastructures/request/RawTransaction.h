//
// Created by Kirill Zhukov on 20.10.2022.
//

#ifndef UNIT_RAWTRANSACTION_H
#define UNIT_RAWTRANSACTION_H

#include <utility>

#include "utility"
#include "sstream"
#include "iostream"
#include "optional"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "../../../global/GlobalVariables.h"
#include "../../utils/StringUtil.h"
#include "../../crypto/SHA3/sha3.h"

namespace json = boost::json;
using namespace boost::multiprecision;

class RawTransaction {
public:
    virtual ~RawTransaction() = default;
    RawTransaction() = default;

    RawTransaction(std::string from,
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

    RawTransaction(std::string from,
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
    uint32_t confirmations{};
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

    inline void setProverData(std::string *signP, std::string *rP, std::string *sP) {
        this->signP = *signP;
        this->signP = *rP;
        this->signP = *sP;
    }

    inline std::shared_ptr<std::string> getHash() {
        return std::make_shared<std::string>(this->hash);
    }

    inline void generateHash() {
        SHA3 sha3 = SHA3(SHA3::Bits256);
        this->hash = sha3(sha3(*this->serializeToRawTransaction()));
    }

    [[nodiscard]] inline std::shared_ptr<std::string> serializeWithoutSignatures() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)" << std::scientific << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)" << this->fee << R"(, "nonce":)" << this->nonce << R"(})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)" << std::scientific
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":)" << RawTransaction::uint256_jv_2string(this->extra.at("value")) << R"(, "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)" << this->nonce << R"(})";
        }
        return std::make_shared<std::string>(ss.str());
    }

    [[nodiscard]] inline std::shared_ptr<std::string> serializeForSending() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << RawTransaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return std::make_shared<std::string>(ss.str());
    }

    [[nodiscard]] inline std::shared_ptr<std::string> serializeToRawTransaction() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << RawTransaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return std::make_shared<std::string>(ss.str());
    }

    [[nodiscard]] inline std::shared_ptr<std::string> serializeToJsonTransaction() const {
        std::stringstream ss;
        if (type == 0)
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "fee":)"
               << this->fee << R"(, "nonce":)" << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        else {
            ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)"
               << this->amount << R"(, "type":)"
               << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << json::value_to<std::string>(this->extra.at("name"))
               << R"(", "value":")" << RawTransaction::uint256_jv_2string(this->extra.at("value")) << R"(", "bytecode": ")" << json::value_to<std::string>(this->extra.at("bytecode"))
               << R"("})" << R"(, "fee":)" << this->fee << R"(, "nonce":)"
               << this->nonce << R"(, "signature":")" << this->sign << R"(", "r":")"
               << this->r << R"(", "s":")" << this->s
               << R"("})";
        }
        return std::make_shared<std::string>(ss.str());
    }

    static std::shared_ptr<RawTransaction> parse(boost::json::value *params) {
        boost::json::object transactionRequestJson = params->as_object();
        transactionRequestJson = transactionRequestJson["data"].as_object();
        if (params->at("type") == 0)
            return std::make_shared<RawTransaction>(RawTransaction(
                    boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("sign")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                    boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                    uint256_t(boost::json::value_to<std::string>(transactionRequestJson.at("amount"))),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("fee")),
                    boost::json::value_to<uint64_t>(transactionRequestJson.at("nonce"))));
        return std::make_shared<RawTransaction>(RawTransaction(
                boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                transactionRequestJson.at("extradata"),
                boost::json::value_to<std::string>(transactionRequestJson.at("sign")),
                boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                uint256_t(boost::json::value_to<std::string>(transactionRequestJson.at("amount"))),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("fee")),
                boost::json::value_to<uint64_t>(transactionRequestJson.at("nonce"))));
    }

    void setConfirmations(uint32_t confirmations) {
        RawTransaction::confirmations = confirmations;
    }

protected:
    inline static std::string uint256_jv_2string(const boost::json::value &value) {
        return boost::json::value_to<std::string>(value);
    }
};
#endif //UNIT_RAWTRANSACTION_H
