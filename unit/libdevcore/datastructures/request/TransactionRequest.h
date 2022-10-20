//
// Created by Kirill Zhukov on 20.10.2022.
//

#ifndef UNIT_TRANSACTIONREQUEST_H
#define UNIT_TRANSACTIONREQUEST_H

#include "iostream"
#include "optional"
#include "boost/json.hpp"
#include "boost/json/value.hpp"
#include "../../../global/GlobalVariables.h"

class TransactionRequest {
public:
    TransactionRequest();
    TransactionRequest(std::string from, std::string to, uint64_t type, uint64_t date,
                       const boost::json::value& extra, std::string hash,
                       std::string sign, double amount, double fee);
    static std::optional<TransactionRequest> parse(std::string *request);

    virtual ~TransactionRequest();

    std::string from;
    std::string to;
    uint64_t type{};
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    boost::json::value extra;
    std::string hash;
    uint64_t block_id{};
    std::string sign;
    double amount{};
    double fee{};
};


#endif //UNIT_TRANSACTIONREQUEST_H
