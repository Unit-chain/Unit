//
// Created by Kirill Zhukov on 20.10.2022.
//

#ifndef UNIT_RAWTRANSACTION_H
#define UNIT_RAWTRANSACTION_H

#include "utility"
#include "sstream"
#include "iostream"
#include "optional"
#include "boost/json.hpp"
#include "sstream"
#include "boost/json/src.hpp"
#include "../../../global/GlobalVariables.h"
#include "../../utils/StringUtil.h"
#include "../../crypto/SHA3/sha3.h"

namespace json = boost::json;

class RawTransaction {
public:
    virtual ~RawTransaction() {
        std::cout << "Destructor" << std::endl;
    }

    RawTransaction() = default;
    RawTransaction(std::string from, std::string to, uint64_t type, uint64_t date,
                   boost::json::value extra,
                   std::string sign, std::string r, std::string s,
                   std::string signP, std::string rP, std::string sP,
                   double amount, double fee) :
            from(std::move(from)), to(std::move(to)),
            type(type), date(date),
            extra(std::move(extra)),
            sign(std::move(sign)),
            r(std::move(r)),
            s(std::move(s)),
            signP(std::move(signP)),
            rP(std::move(rP)),
            sP(std::move(sP)),
            amount(amount),
            fee(fee) {}

    RawTransaction(std::string from,
                   std::string to,
                   uint64_t type,
                   json::value extra,
                   std::string sign,
                   std::string r,
                   std::string s,
                   double amount,
                   double fee) :
            from(std::move(from)),
            to(std::move(to)),
            type(type),
            extra(std::move(extra)),
            sign(std::move(sign)),
            r(std::move(r)),
            s(std::move(s)),
            amount(amount),
            fee(fee) {}

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
    double amount{};
    double fee{};

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

    [[nodiscard]] inline std::shared_ptr<std::string> serializeToRawTransaction() const {
        auto name = json::value_to<std::string>(this->extra.at("name"));
        auto value = json::value_to<double>(this->extra.at("value"));
        auto bytecode = json::value_to<std::string>(this->extra.at("bytecode"));
        std::stringstream ss;
        ss << R"({"from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)" << std::scientific << this->amount << R"(, "type":)"
           << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << name << R"(", "value":)" << value << R"(, "bytecode": ")" << bytecode
           << R"("}, "sign":")" << this->sign << R"(", "r":")" << this->r << R"(", "s":")" << this->s << R"(", "signP":")" << this->signP << R"(", "rP":")" << this->rP
           << R"(", "sP":")" << this->sP << R"(", "fee":)" << this->fee << R"(})";
        return std::make_shared<std::string>(ss.str());
    }

    [[nodiscard]] inline std::shared_ptr<std::string> serializeToJsonTransaction() const {
        auto name = json::value_to<std::string>(this->extra.at("name"));
        auto value = json::value_to<double>(this->extra.at("value"));
        auto bytecode = json::value_to<std::string>(this->extra.at("bytecode"));
        std::stringstream ss;
        ss << R"({"hash":")" << this->hash << R"(", "from":")" << this->from << R"(", "to":")" << this->to << R"(", "amount":)" << std::scientific << this->amount << R"(, "type":)"
        << this->type << R"(, "date":)" << this->date << R"(, "extradata":{)" << R"("name":")" << name << R"(", "value":)" << value << R"(, "bytecode": ")" << bytecode
        << R"("}, "sign":")" << this->sign << R"(", "r":")" << this->r << R"(", "s":")" << this->s << R"(", "signP":")" << this->signP << R"(", "rP":")" << this->rP
        << R"(", "sP":")" << this->sP << R"(", "fee":)" << this->fee << R"(})";
        return std::make_shared<std::string>(ss.str());
    }

    static RawTransaction *parse(std::string *request) {
        json::error_code ec;
        try {
            boost::json::object transactionRequestJson = json::parse(*request, ec).as_object();
            transactionRequestJson = transactionRequestJson["data"].as_object();
            if (!transactionRequestJson.contains("from") || !transactionRequestJson.contains("to")
                || !transactionRequestJson.contains("type") || !transactionRequestJson.contains("extradata")
                || !transactionRequestJson.contains("sign") || !transactionRequestJson.contains("r") || !transactionRequestJson.contains("s")
                || !transactionRequestJson.contains("amount") || !transactionRequestJson.contains("fee"))
                return nullptr;
            return new RawTransaction(
                                  boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                                  boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                                  transactionRequestJson.at("extradata"),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("sign")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                                  boost::json::value_to<double>(transactionRequestJson.at("amount")),
                                  boost::json::value_to<double>(transactionRequestJson.at("fee")));
        } catch (const boost::exception &o) {
            logger <<  "RawTransaction.h parsing error: " << ec.message() << std::endl;
            return nullptr;
        }
    }

    [[deprecated("Please use \"RawTransaction *parse(...)\"")]]
    static std::shared_ptr<RawTransaction> parseOld(std::string *request) {
        json::error_code ec;
        try {
            boost::json::object transactionRequestJson = json::parse(*request, ec).as_object();
            transactionRequestJson = transactionRequestJson["data"].as_object();
            if (!transactionRequestJson.contains("from") || !transactionRequestJson.contains("to")
                || !transactionRequestJson.contains("type") || !transactionRequestJson.contains("extradata")
                || !transactionRequestJson.contains("sign") || !transactionRequestJson.contains("r") || !transactionRequestJson.contains("s")
                || !transactionRequestJson.contains("amount") || !transactionRequestJson.contains("fee"))
                return nullptr;
            return std::make_shared<RawTransaction>(RawTransaction(
                                  boost::json::value_to<std::string>(transactionRequestJson.at("from")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("to")),
                                  boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
                                  transactionRequestJson.at("extradata"),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("sign")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("r")),
                                  boost::json::value_to<std::string>(transactionRequestJson.at("s")),
                                  boost::json::value_to<double>(transactionRequestJson.at("amount")),
                                  boost::json::value_to<double>(transactionRequestJson.at("fee"))));
        } catch (const boost::exception &o) {
            logger << "################## RawTransaction.h parsing error: " << ec.message() << std::endl;
            return nullptr;
        }
    }

    void setConfirmations(uint32_t confirmations) {
        RawTransaction::confirmations = confirmations;
    }
};
#endif //UNIT_RAWTRANSACTION_H
