//
// Created by Kirill Zhukov on 21.10.2022.
//

#ifndef UNIT_VALIDTRANSACTION_H
#define UNIT_VALIDTRANSACTION_H

#include "utility"
#include "iostream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "../../request/RawTransaction.h"

namespace json = boost::json;

class ValidTransaction {
public:
    ValidTransaction() = default;
    explicit ValidTransaction(RawTransaction *rawPointer) : to(rawPointer->to), type(rawPointer->type), date(rawPointer->date), extra(rawPointer->extra),
                                                                   hash(rawPointer->hash),
                                                                   sign(rawPointer->sign), r(rawPointer->r), s(rawPointer->s),
                                                                   signP(rawPointer->signP), rP(rawPointer->rP), sP(rawPointer->sP),
                                                                   amount(rawPointer->amount), fee(rawPointer->fee) {}
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
    double amount{};
    double fee{};

    inline std::shared_ptr<std::string> getHash() {
        return std::make_shared<std::string>(this->hash);
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

    [[nodiscard]] inline uint32_t getSize() const {
        return static_cast<uint32_t>((this->serializeToJsonTransaction())->size());
    }

    void setIndex(uint32_t index) {
        ValidTransaction::index = index;
    }

    [[deprecated("Not implemented yet")]]
    void setZKProveData() {}
};


#endif //UNIT_VALIDTRANSACTION_H
