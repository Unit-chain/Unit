//
// Created by Kirill Zhukov on 11.11.2022.
//

#ifndef UNIT_RPCEXCEPTIONS_H
#define UNIT_RPCEXCEPTIONS_H
#include "iostream"
#include "ServerIncludes.h"

class RpcMethodSupportException : public std::exception {
public:
    std::string what() {
        return rpcError::httpMethodError;
    }
};

class RpcParseException : public std::exception {
public:
    std::string what() {
        return rpcError::parsingError;
    }
};

class RpcEmptyBalanceException : public std::exception {
public:
    std::string what() {
        return rpcError::emptyBalanceError;
    }
};

class RpcDefaultException : public std::exception {
public:
    std::string what() {
        return rpcError::defaultError;
    }
};

class RpcDefaultAccountException : public std::exception {
public:
    std::string what() {
        return rpcError::defaultAccountError;
    }
};

class RpcBadNonceException : public std::exception {
public:
    std::string what() {
        return rpcError::badNonce;
    }
};

class RpcInvalidSignatureException : public std::exception {
public:
    std::string what() {
        return rpcError::invalidSignature;
    }
};

class RpcLowBalanceException : public std::exception {
public:
    std::string what() {
        return rpcError::lowBalance;
    }
};

class RpcInvalidParameterException : public std::exception {
public:
    std::string what() {
        return rpcError::invalidParameter;
    }
};
#endif //UNIT_RPCEXCEPTIONS_H
