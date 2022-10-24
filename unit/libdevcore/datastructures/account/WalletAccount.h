//
// Created by Kirill Zhukov on 23.10.2022.
//

#ifndef UNIT_WALLETACCOUNT_H
#define UNIT_WALLETACCOUNT_H
#include "../../../global/GlobalVariables.h"
#include "../../../global/errors//WalletErrors.h"
#include <utility>
#include "iostream"
#include "vector"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/json/array.hpp"
#include "sstream"

namespace json = boost::json;

class WalletAccount {
public:
    WalletAccount() = default;
    WalletAccount(std::string address, double balance, boost::json::object tokensBalance, json::array txOutputs,
                  json::array txInputs) : address(std::move(address)), balance(balance), txOutputs(txOutputs.storage()),
                                                              txInputs(txInputs.storage()), tokensBalance(std::move(tokensBalance)) {}
    WalletAccount(std::string address, double balance, json::object tokensBalance);
    virtual ~WalletAccount() = default;

    std::string address;
    double balance{};
    json::object tokensBalance{};
    json::array txOutputs{};
    json::array txInputs{};
    bool changed = false;

    static std::optional<WalletAccount*> parseWallet(std::string *ptr);
    bool parseHistory(std::string *ptr);
    operationStatus::WalletErrorsCode subtract(double value, const std::string& inputHash);
    operationStatus::WalletErrorsCode subtractToken(double value, const std::string& inputHash, const std::string &tokenName);
    void increase(double value, const std::string& inputHash);
    void increaseToken(double value, const std::string& inputHash, const std::string &tokenName);
    void setTxOutputs(const json::array &txOutputs);
    void setTxInputs(const json::array &txInputs);
    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::string serializeHistory() const;
};

std::optional<WalletAccount*> WalletAccount::parseWallet(std::string *ptr) {
    if (ptr == nullptr) return std::nullopt;
    json::error_code ec;
    try {
        json::value value = json::parse(*ptr, ec);
        auto address = boost::json::value_to<std::string>(value.at("address"));
        auto tokensBalance = value.at("tokensBalance").as_object();
        return new WalletAccount(address, value.at("balance").as_double(), tokensBalance);
    } catch (const boost::exception &o) {
        logger << ec.message() << std::endl;
        return std::nullopt;
    }
}

std::string WalletAccount::serialize() const {
    std::stringstream ss;
    ss << R"({"address":")" << this->address << R"(", "balance":)" << std::scientific << this->balance << ((tokensBalance.empty()) ? "}" : ", ");
    if (!tokensBalance.empty()) {
        ss << R"("tokensBalance":)" << boost::json::serialize(tokensBalance) << "}";
    }
    return ss.str();
}

void WalletAccount::increase(double value, const std::string& inputHash) {
    this->balance += value;
    this->txInputs.emplace_back(inputHash);
    this->changed = true;
}

operationStatus::WalletErrorsCode WalletAccount::subtract(double value, const std::string& inputHash) {
    if (this->balance < value) return operationStatus::WalletErrorsCode::cLowNativeTokenBalance;
    this->balance -= value;
    this->txOutputs.emplace_back(inputHash);
    this->changed = true;
    return operationStatus::WalletErrorsCode::cOk;
}

operationStatus::WalletErrorsCode WalletAccount::subtractToken(double value, const std::string &inputHash, const std::string &tokenName) {
    if (!this->tokensBalance.contains(tokenName)) return operationStatus::WalletErrorsCode::cTokenBalanceNotFound;
    if (this->tokensBalance[tokenName].as_double() < value) return operationStatus::WalletErrorsCode::cLowTokenBalance;
    this->tokensBalance[tokenName] = this->tokensBalance[tokenName].as_double() - value;
    this->changed = true;
    return operationStatus::WalletErrorsCode::cOk;
    #if 0
            boost::json::object object = boost::json::parse(R"({"test":{}})").as_object();
            object["test"].as_object()["token"] = 12123.123;
            object["test"].as_object()["token2"] = 123.123;
            object["test"].as_object()["token3"] = 123.123;
            auto str = boost::json::serialize(object);
            std::cout << str << std::endl;
    #endif
}

void WalletAccount::increaseToken(double value, const std::string &inputHash, const std::string &tokenName) {
    if (!this->tokensBalance.contains(tokenName)) {
        this->tokensBalance[tokenName] = value;
        this->changed = true;
        return;
    }
    this->tokensBalance[tokenName] = this->tokensBalance[tokenName].as_double() + value;
    this->changed = true;
}

WalletAccount::WalletAccount(std::string address, double balance, json::object tokensBalance) : address(std::move(
        address)), balance(balance), tokensBalance(std::move(tokensBalance)) {}

bool WalletAccount::parseHistory(std::string *ptr) {
    if (ptr == nullptr) return false;
    json::error_code ec;
    try {
        json::value value = json::parse(*ptr, ec);
        std::cout << value << std::endl;
        this->txOutputs = value.at("out").as_array();
        this->txInputs = value.at("in").as_array();
        return true;
    } catch (const boost::exception &o) {
        logger << ec.message() << std::endl;
        return false;
    }
}

void WalletAccount::setTxOutputs(const json::array &txOutputs) {
    WalletAccount::txOutputs = boost::json::array(txOutputs.storage());
}

void WalletAccount::setTxInputs(const json::array &txInputs) {
    WalletAccount::txInputs = boost::json::array(txInputs.storage());
}

std::string WalletAccount::serializeHistory() const {
    std::stringstream ss;
    ss << R"({"out":)" << this->txOutputs << R"(, "in":)" << this->txInputs << R"(})";
    return ss.str();
}


#endif //UNIT_WALLETACCOUNT_H
