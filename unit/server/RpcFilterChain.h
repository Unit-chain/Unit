//
// Created by Kirill Zhukov on 11.11.2022.
//

#ifndef UNIT_RPCFILTERCHAIN_H
#define UNIT_RPCFILTERCHAIN_H
#include <utility>

#include "tuple"
#include "iostream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"

#include "RpcExceptions.h"
#include "ServerIncludes.h"
#include "../pools/TransactionPool.h"
#include "../libdevcore/bip44/ecdsa.hpp"
#include "../libdevcore/datastructures/account/WalletAccount.h"
#include "../libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"

class RpcFilterChain {
public:
    RpcFilterChain() = default;
    RpcFilterChain *add(RpcFilterChain *n);
    RpcFilterChain *setNext(RpcFilterChain *n);
    virtual void filter(boost::json::value *json);
private:
    RpcFilterChain *next;
};

RpcFilterChain *RpcFilterChain::add(RpcFilterChain *n) {
    if (this->next) this->next->add(n);
    else next = n;
    return this;
}

RpcFilterChain *RpcFilterChain::setNext(RpcFilterChain *n) {
    this->next = n;
    return this;
}

void RpcFilterChain::filter(boost::json::value *json) {
    if (this->next) this->next->filter(json);
}

class BasicTransactionRpcFilter : public RpcFilterChain {
public:
    BasicTransactionRpcFilter(unit::DB *userProvider, http::response<http::dynamic_body> *response, TransactionPool *transactionPool)
            : userProvider(userProvider), response(response), transactionPool(transactionPool) {}
    void filter(boost::json::value *parameter) override;
protected:
    unit::DB *userProvider;
    http::response<http::dynamic_body> *response;
    TransactionPool *transactionPool;
};

void BasicTransactionRpcFilter::filter(boost::json::value *parameter) {
    RawTransaction rawTransaction = RawTransaction::parse(parameter);
//    std::cout << *parameter << std::endl;
    auto sender = boost::json::value_to<std::string>(parameter->at("params").at("from"));
    std::variant<std::string, std::exception> dbResponse = this->userProvider->get(sender);
    if (std::holds_alternative<std::exception>(dbResponse)) {
        create_error_response(rpcError::emptyBalanceError, this->response);
        throw RpcDefaultException();
    }
    std::optional<WalletAccount> opAccount = WalletAccount::parseWallet(&std::get<0>(dbResponse));
    if (!opAccount.has_value()) {
        create_error_response(rpcError::defaultAccountError, this->response);
        throw RpcEmptyBalanceException();
    }
    std::optional<WalletAccount> account = opAccount.value();
    if (account->nonce == boost::json::value_to<uint64_t>(parameter->at("params").at("nonce"))) {
        create_error_response(rpcError::badNonce, this->response);
        throw RpcBadNonceException();
    }
    if (!ecdsa_verify_signature(boost::json::value_to<std::string>(parameter->at("params").at("r")), boost::json::value_to<std::string>(parameter->at("params").at("s")),
                                rawTransaction.serializeWithoutSignatures(), boost::json::value_to<std::string>(parameter->at("params").at("from")))) {
        create_error_response(rpcError::invalidSignature, this->response);
        throw RpcInvalidSignatureException();
    }
    if (0 == boost::json::value_to<int>(parameter->at("params").at("type")) && (account->compareNativeTokenBalance(parameter->at("params").at("amount")) < 0)) {
        create_error_response(rpcError::lowBalance, this->response);
        throw RpcLowBalanceException();
    }
    if (1 == boost::json::value_to<int>(parameter->at("params").at("type")) && (account->compareTokenBalance(parameter->at("params").at("amount"), parameter->at("params").at("extradata").at("name")) < 0)) {
        create_error_response(rpcError::lowBalance, this->response);
        throw RpcLowBalanceException();
    }
    rawTransaction.generateHash();
    this->transactionPool->emplaceBack(ValidTransaction(rawTransaction));
    int id = boost::json::value_to<int>(parameter->at("id"));
    create_success_response(rpcResponse::processSimpleStringResponse(rawTransaction.hash, id), this->response);
}

/// Used for process balance request
class BasicBalanceFilter : public RpcFilterChain {
public:
    BasicBalanceFilter(unit::DB *userProvider, http::response<http::dynamic_body> *response) : userProvider(userProvider), response(response) {}
    void filter(boost::json::value *parameter) override;
protected:
    unit::DB *userProvider;
    http::response<http::dynamic_body> *response;
};

void BasicBalanceFilter::filter(boost::json::value *parameter) {
    if (!parameter->as_object().contains("params")) throw RpcInvalidParameterException();
    auto param = boost::json::value_to<std::string>(parameter->at("params"));
    std::variant<std::string, std::exception> dbResponse = this->userProvider->get(param);
    unit::DB::isSameError<decltype(std::get<1>(dbResponse)), unit::error::DBNotFound>(std::get<1>(dbResponse));
    if (std::holds_alternative<std::exception>(dbResponse)) (unit::DB::isSameError<decltype(std::get<1>(dbResponse)), unit::error::DBNotFound>(std::get<1>(dbResponse))) ? throw RpcEmptyBalanceException() : throw RpcDefaultException();
    create_success_response(rpcResponse::processSimpleResponse(std::get<0>(dbResponse), boost::json::value_to<std::string>(parameter->at("id"))), this->response);
}

/// Used for process pool size request
class BasicPoolFilter : public RpcFilterChain {
public:
    BasicPoolFilter(http::response<http::dynamic_body> *response, TransactionPool *transactionPool) : response(response), transactionPool(transactionPool) {}
    void filter(boost::json::value *parameter) override;
protected:
    http::response<http::dynamic_body> *response;
    TransactionPool *transactionPool;
};

void BasicPoolFilter::filter(boost::json::value *parameter) {
    create_success_response(rpcResponse::processSimpleResponse(std::to_string(this->transactionPool->getPoolSize()), boost::json::value_to<std::string>(parameter->at("id"))), this->response);
}

/// Used for process balance history request
class BasicBalanceHistoryFilter : public RpcFilterChain {
public:
    BasicBalanceHistoryFilter(http::response<http::dynamic_body> *response, unit::DB *historyDB) : response(response), historyDB(historyDB) {}
    void filter(boost::json::value *parameter) override;
protected:
    http::response<http::dynamic_body> *response;
    unit::DB *historyDB;
};

void BasicBalanceHistoryFilter::filter(boost::json::value *parameter) {
    if (!parameter->as_object().contains("params")) throw RpcInvalidParameterException();
    auto sender = boost::json::value_to<std::string>(parameter->at("params"));
    std::variant<std::string, std::exception> dbResponse = this->historyDB->get(sender);
    if (std::holds_alternative<std::exception>(dbResponse)) (unit::DB::isSameError<decltype(std::get<1>(dbResponse)), unit::error::DBNotFound>(std::get<1>(dbResponse))) ? throw RpcEmptyBalanceException() : throw RpcDefaultException();
    create_success_response(rpcResponse::processSimpleResponse(std::get<0>(dbResponse), boost::json::value_to<std::string>(parameter->at("id"))), this->response);
}

/// Used for process current block request
class BasicBlockHeightFilter : public RpcFilterChain {
public:
    BasicBlockHeightFilter(http::response<http::dynamic_body> *response, unit::DB *blockProvider) : response(response), blockProvider(blockProvider) {}
    void filter(boost::json::value *parameter) override;
protected:
    http::response<http::dynamic_body> *response;
    unit::DB *blockProvider;
    std::string key = "current";
};

void BasicBlockHeightFilter::filter(boost::json::value *parameter) {
    create_success_response(rpcResponse::processSimpleResponse(this->blockProvider->get(this->key), boost::json::value_to<std::string>(parameter->at("id"))), this->response);
}

/// Used for process transaction by hash request
class BasicTransactionByHashRpcFilter : public RpcFilterChain {
public:
    BasicTransactionByHashRpcFilter(http::response<http::dynamic_body> *response, unit::DB *transactionDB) : response(response), transactionDB(transactionDB) {}
    void filter(boost::json::value *parameter) override;
protected:
    http::response<http::dynamic_body> *response;
    unit::DB *transactionDB;
};

void BasicTransactionByHashRpcFilter::filter(boost::json::value *parameter) {
    if (!parameter->as_object().contains("params")) throw RpcInvalidParameterException();
    auto sender = boost::json::value_to<std::string>(parameter->at("params"));
    std::variant<std::string, std::exception> dbResponse = this->transactionDB->get(sender);
    if (std::holds_alternative<std::exception>(dbResponse)) (unit::DB::isSameError<decltype(std::get<1>(dbResponse)), unit::error::DBNotFound>(std::get<1>(dbResponse))) ? throw RpcEmptyBalanceException() : throw RpcDefaultException();
    create_success_response(rpcResponse::processSimpleResponse(std::get<0>(dbResponse), boost::json::value_to<std::string>(parameter->at("id"))), this->response);
}

#endif //UNIT_RPCFILTERCHAIN_H
