//
// Created by Kirill Zhukov on 07.11.2022.
//

#ifndef UNIT_RPCMETHOD_H
#define UNIT_RPCMETHOD_H
#include "variant"
#include "iostream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "account/WalletAccount.h"
#include "bip44/ecdsa.hpp"

class RpcMethod {
public:
    virtual std::shared_ptr<boost::json::value> validateRequest(boost::json::value *params) = 0;
};

class RpcMethodHandler {
public:
    RpcMethodHandler() = default;
    explicit RpcMethodHandler(RpcMethod *method) : method(method) {}

    [[nodiscard]] const std::shared_ptr<RpcMethod> &getMethod() const {return method;}
    void setMethod(const std::shared_ptr<RpcMethod> &method) { RpcMethodHandler::method = method;}

    /// should return nullptr if ok
    /// or else should return error
    inline std::shared_ptr<boost::json::value> executeValidating(boost::json::value *params) { return this->method->validateRequest(params); }
private:
    std::shared_ptr<RpcMethod> method;
};

class TransferMethod : public RpcMethod {
    std::shared_ptr<boost::json::value> validateRequest(boost::json::value *params) override;
};

std::shared_ptr<boost::json::value> TransferMethod::validateRequest(boost::json::value *params) {
    boost::json::value responseJSON = boost::json::parse(R"({"jsonrpc": "2.0"})");
    if (!params->as_object().contains("from") || !params->as_object().contains("to") || !params->as_object().contains("amount") || !params->as_object().contains("type") ||
    !params->as_object().contains("r") || !params->as_object().contains("s") || !params->as_object().contains("signature")
        || !params->as_object()["extradata"].as_object().contains("value")) {
        responseJSON.as_object()["error"].as_object().emplace("code", -32602);
        responseJSON.as_object()["error"].as_object().emplace("message", "request doesn't have enough fields");
        responseJSON.as_object()["error"].as_object().emplace("id", "null");
        return std::make_shared<boost::json::value>(responseJSON);
    }
    if (params->at("type").as_int64() == 1 && (!params->as_object().contains("extradata") || !params->as_object()["extradata"].as_object().contains("name")
                                               || !params->as_object()["extradata"].as_object().contains("value"))) {
        responseJSON.as_object()["error"].as_object().emplace("code", -32602);
        responseJSON.as_object()["error"].as_object().emplace("message", "request for transfer tokens should contains \'extradata\' field");
        responseJSON.as_object()["error"].as_object().emplace("id", "null");
        return std::make_shared<boost::json::value>(responseJSON);
    }
    return nullptr;
}

#endif //UNIT_RPCMETHOD_H


//std::shared_ptr<Transaction> rawTransaction = Transaction::parse(params);
//if (!ecdsa_verify_signature(boost::json::value_to<std::string>(params->at("r")), boost::json::value_to<std::string>(params->at("s")),
//        *rawTransaction->serializeWithoutSignatures(), boost::json::value_to<std::string>(params->at("from")))) {
//responseJSON.as_object()["error"].as_object().emplace("code", -32602);
//responseJSON.as_object()["error"].as_object().emplace("message", "invalid signature");
//responseJSON.as_object()["error"].as_object().emplace("id", "null");
//return responseJSON;
//}
//return rawTransaction;