//
// Created by Kirill Zhukov on 07.11.2022.
//

#ifndef UNIT_RPCMETHOD_H
#define UNIT_RPCMETHOD_H
#include "iostream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "../libdevcore/datastructures/account/WalletAccount.h"

class RpcMethod {
public:
    virtual boost::json::value validateRequest(boost::json::value *params);
};

class TransferMethod : public RpcMethod {
    boost::json::value validateRequest(boost::json::value *params) override;
};

boost::json::value TransferMethod::validateRequest(boost::json::value *params) {
    boost::json::value responseJSON = boost::json::parse(R"({"jsonrpc": "2.0"})");
    if (!params->as_object().contains("from") || !params->as_object().contains("to") || !params->as_object().contains("amount") || !params->as_object().contains("type") ||
    !params->as_object().contains("r") || !params->as_object().contains("s") || !params->as_object().contains("signature")
        || !params->as_object()["extradata"].as_object().contains("value")) {
        responseJSON.as_object()["error"].as_object().emplace("code", -32602);
        responseJSON.as_object()["error"].as_object().emplace("message", "request doesn't have enough fields");
        responseJSON.as_object()["error"].as_object().emplace("id", "null");
        return responseJSON;
    }
    if (params->at("type").as_int64() == 1 && (!params->as_object().contains("extradata") || !params->as_object()["extradata"].as_object().contains("name")
                                               || !params->as_object()["extradata"].as_object().contains("value"))) {
        responseJSON.as_object()["error"].as_object().emplace("code", -32602);
        responseJSON.as_object()["error"].as_object().emplace("message", "request for transfer tokens should contains \'extradata\' field");
        responseJSON.as_object()["error"].as_object().emplace("id", "null");
        return responseJSON;
    }
    return "test";
}

#endif //UNIT_RPCMETHOD_H
