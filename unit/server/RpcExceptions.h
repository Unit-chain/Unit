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
        return messageError::httpMethodError;
    }
};

class RpcParseException : public std::exception {
public:
    std::string what() {
        return messageError::parsingError;
    }
};

class RpcEmptyBalanceException : public std::exception {
public:
    std::string what() {
        return messageError::emptyBalanceError;
    }
};

class RpcDefaultException : public std::exception {
public:
    std::string what() {
        return messageError::defaultError;
    }
};

class RpcDefaultAccountException : public std::exception {
public:
    std::string what() {
        return messageError::defaultAccountError;
    }
};

class RpcBadNonceException : public std::exception {
public:
    std::string what() {
        return messageError::badNonce;
    }
};

class RpcInvalidSignatureException : public std::exception {
public:
    std::string what() {
        return messageError::invalidSignature;
    }
};

class RpcLowBalanceException : public std::exception {
public:
    std::string what() {
        return messageError::lowBalance;
    }
};

class RpcInvalidParameterException : public std::exception {
public:
    std::string what() {
        return messageError::invalidParameter;
    }
};
#endif //UNIT_RPCEXCEPTIONS_H
