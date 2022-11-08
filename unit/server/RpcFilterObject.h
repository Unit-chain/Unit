//
// Created by Kirill Zhukov on 08.11.2022.
//

#ifndef UNIT_RPCFILTEROBJECT_H
#define UNIT_RPCFILTEROBJECT_H
#include "iostream"
#include "tuple"
#include "ServerIncludes.h"
#include "../libdevcore/datastructures/request/RawTransaction.h"
#include "../libdevcore/datastructures/account/WalletAccount.h"
#include "../libdevcore/db/DBProvider.h"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../libdevcore/bip44/BIP32.hpp"
#include "../libdevcore/bip44/BIP39.hpp"
#include "../libdevcore/bip44/BIP44.hpp"
#include "../libdevcore/bip44/utils.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

inline void create_error_response(const std::string &message, http::response<http::dynamic_body> *response_) {
    response_->result(http::status::bad_request);
    response_->set(http::field::content_type, "application/json");
    response_->set(http::field::server, "Unit");
    beast::ostream(response_->body()) << message;
}

class RpcFilterObject {
public:
    virtual std::tuple<bool, std::shared_ptr<boost::json::value>> doFilterInternal(std::shared_ptr<boost::json::value> parameter);
};

class BasicTransactionFilter : public RpcFilterObject {
public:
    BasicTransactionFilter();
    explicit BasicTransactionFilter(dbProvider::BatchProvider *userProvider, http::response<http::dynamic_body> *response);
    std::tuple<bool, std::shared_ptr<boost::json::value>> doFilterInternal(std::shared_ptr<boost::json::value> parameter) override;
private:
    dbProvider::BatchProvider *userProvider;
    http::response<http::dynamic_body> *response;
};

std::tuple<bool, std::shared_ptr<boost::json::value>> BasicTransactionFilter::doFilterInternal(std::shared_ptr<boost::json::value> parameter) {
    std::shared_ptr<RawTransaction> rawTransaction = RawTransaction::parse(parameter.get());
    auto sender = boost::json::value_to<std::string>(parameter->at("from"));
    operationDBStatus::DBResponse<std::string> dbResponse = this->userProvider->read<std::string>(&sender);
    if (dbResponse.error && ((int) dbResponse.errorResponse) == 2) {
        create_error_response(rpcError::emptyBalanceError, this->response);
        return {false, nullptr};
    }
    std::optional<std::shared_ptr<WalletAccount>> opAccount = WalletAccount::parseWallet(dbResponse.value);
    if (!opAccount.has_value()) {
        create_error_response(rpcError::defaultAccountError, this->response);
        return {false, nullptr};
    }
    std::shared_ptr<WalletAccount> account = opAccount.value();
    if (account->nonce == parameter->at("nonce")) {
        create_error_response(rpcError::badNonce, this->response);
        return {false, nullptr};
    }
    if (!ecdsa_verify_signature(boost::json::value_to<std::string>(parameter->at("r")), boost::json::value_to<std::string>(parameter->at("s")),
                                *rawTransaction->serializeWithoutSignatures(), boost::json::value_to<std::string>(parameter->at("from")))) {
        create_error_response(rpcError::invalidSignature, this->response);
        return {false, nullptr};
    }
    if (boost::json::value_to<int>(parameter->at("type")) == 0 && (account->compareNativeTokenBalance(parameter->at("amount")) < 0)) {
        create_error_response(rpcError::lowBalance, this->response);
        return {false, nullptr};
    }
    if (boost::json::value_to<int>(parameter->at("type")) == 1 && (account->compareTokenBalance(parameter->at("amount"), parameter->at("extradata").at("name")) < 0)) {
        create_error_response(rpcError::lowBalance, this->response);
        return {false, nullptr};
    }
    return {true, nullptr};
}

BasicTransactionFilter::BasicTransactionFilter() {}

BasicTransactionFilter::BasicTransactionFilter(dbProvider::BatchProvider *userProvider, http::response<http::dynamic_body> *response) :
                                               userProvider(userProvider),
                                               response(response) {}

#endif //UNIT_RPCFILTEROBJECT_H
