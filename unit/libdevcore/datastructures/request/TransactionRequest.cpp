//
// Created by Kirill Zhukov on 20.10.2022.
//

#include "TransactionRequest.h"
#include <utility>

TransactionRequest::~TransactionRequest() = default;
TransactionRequest::TransactionRequest() = default;
TransactionRequest::TransactionRequest(std::string from, std::string to, uint64_t type, uint64_t date,
                                       const boost::json::value& extra, std::string hash,
                                       std::string sign, double amount, double fee) : from(std::move(from)), to(std::move(to)),
                                                                                             type(type), date(date),
                                                                                             extra(extra.storage()), hash(std::move(hash)),
                                                                                             sign(std::move(sign)), amount(amount),
                                                                                             fee(fee) {}

std::optional<TransactionRequest> TransactionRequest::parse(std::string *request) {
//    boost::json::object transactionRequestJson;
    return std::nullopt;
//    try {
//        transactionRequestJson = boost::json::parse(*request).as_object();
//    } catch (const boost::exception &o) {
//        logger << "transaction request parsing error." << std::endl;
//        return std::nullopt;
//    }
//
//    transactionRequestJson = transactionRequestJson["data"].as_object();
//    if (!transactionRequestJson.contains("from") || !transactionRequestJson.contains("to")
//    || !transactionRequestJson.contains("type") || !transactionRequestJson.contains("date")
//    || !transactionRequestJson.contains("extra") || !transactionRequestJson.contains("hash")
//    || !transactionRequestJson.contains("sign") || !transactionRequestJson.contains("amount")
//    || !transactionRequestJson.contains("fee")) return std::nullopt;
//
//    return TransactionRequest(boost::json::value_to<std::string>(transactionRequestJson.at("from")),
//                                        boost::json::value_to<std::string>(transactionRequestJson.at("to")),
//                                        boost::json::value_to<uint64_t>(transactionRequestJson.at("type")),
//                                        boost::json::value_to<uint64_t>(transactionRequestJson.at("date")),
//                                            transactionRequestJson.at("extra"),
//                                        boost::json::value_to<std::string>(transactionRequestJson.at("hash")),
//                                        boost::json::value_to<std::string>(transactionRequestJson.at("sign")),
//                                        boost::json::value_to<double>(transactionRequestJson.at("amount")),
//                                        boost::json::value_to<double>(transactionRequestJson.at("fee")));
}
