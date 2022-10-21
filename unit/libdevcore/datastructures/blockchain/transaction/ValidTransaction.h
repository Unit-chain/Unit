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
    double amount{};
    double fee{};

    inline std::shared_ptr<std::string> getHash() {
        return std::make_shared<std::string>(this->hash);
    }

    inline std::shared_ptr<std::string> serializeToRawTransaction() {
        std::string mainString = R"({"from":"%s", "to":"%s", "amount":%d, "type":%d, "date":%d, "extradata": {"name":"%s", "value":"%s", "bytecode":"%s"},
        "sign":"%s", "r":"%s", "s":"%s", "signP":"%s", "rP":"%s", "sP":"%s", "fee":%d})";
        auto name = json::value_to<std::string>(this->extra.at("name"));
        auto value = json::value_to<std::string>(this->extra.at("value"));
        auto bytecode = json::value_to<std::string>(this->extra.at("bytecode"));
        return StringUtil::insertSubElement(&mainString, &(this->from), &(this->to), &(this->amount),
                                            &(this->type), &(this->date), &(name), &(value), &(bytecode),
                                            &(this->sign), &(this->r), &(this->s),
                                            &(this->signP), &(this->rP), &(this->sP), &(this->fee));
    }

    [[deprecated("Not implemented yet")]]
    void setZKProveData() {}
};


#endif //UNIT_VALIDTRANSACTION_H
