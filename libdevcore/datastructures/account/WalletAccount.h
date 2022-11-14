//
// Created by Kirill Zhukov on 23.10.2022.
//

#ifndef UNIT_WALLETACCOUNT_H
#define UNIT_WALLETACCOUNT_H
#include "utility"
#include "iostream"
#include "vector"
#include "sstream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/json/array.hpp"
#include "../../../global/GlobalVariables.h"
#include "../../../global/errors/WalletErrors.h"
#include "../../bip44/utils.hpp"
#include "../request/RawTransaction.h"
#include "AbstractAccount.h"


namespace json = boost::json;
using namespace boost::multiprecision;

class WalletAccount : public AbstractAccount {
public:
    WalletAccount() = default;
    WalletAccount(std::string address, uint256_t balance, boost::json::object tokensBalance, const json::array& txOutputs,
                  const json::array& txInputs, uint64_t nonce) : address(std::move(address)), balance(std::move(balance)), txOutputs(txOutputs.storage()),
                                                              txInputs(txInputs.storage()), tokensBalance(std::move(tokensBalance)), nonce(nonce) {}
    WalletAccount(std::string address, uint256_t balance, json::object tokensBalance);
    WalletAccount(std::string address, uint256_t balance, json::object tokensBalance, uint64_t nonce);
    explicit WalletAccount(std::string address) : address(std::move(address)) {
        this->balance = 0;
        this->nonce = 0;
    }
    virtual ~WalletAccount() = default;

    std::string address;
    uint256_t balance{};
    uint64_t nonce;
    json::object tokensBalance{};
    json::array txOutputs{};
    json::array txInputs{};
    bool changed = false;

    static std::optional<std::shared_ptr<WalletAccount>> parseWallet(std::string *ptr);
    static WalletAccount* createEmptyWallet(const std::string &);
    bool parseHistory(std::string *ptr);
    operationStatus::WalletErrorsCode subtract(const uint256_t& value, const std::string& outputHash);
    operationStatus::WalletErrorsCode subtractToken(const uint256_t& value, const std::string& inputHash, const std::string &tokenName);
    void increase(const uint256_t& value, const std::string& inputHash);
    void increaseToken(const uint256_t& value, const std::string& inputHash, const std::string &tokenName);
    void setTxOutputs(const json::array &txOutputs);
    void setTxInputs(const json::array &txInputs);
    bool isValidNonce(RawTransaction *rawPointer) const;
    int compareNativeTokenBalance(const json::value &amount) const;
    int compareTokenBalance(const json::value &amount, const json::value &tokenName);
    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::string serializeHistory() const;
};

std::optional<std::shared_ptr<WalletAccount>> WalletAccount::parseWallet(std::string *ptr) {
    if (ptr == nullptr) return std::nullopt;
    json::error_code ec;
    try {
        json::value value = json::parse(*ptr, ec);
        auto address = boost::json::value_to<std::string>(value.at("address"));
        auto tokensBalance = value.at("tokensBalance").as_object();
        return std::make_shared<WalletAccount>(WalletAccount(address, uint256_t(boost::json::value_to<std::string>(value.at("balance"))), tokensBalance, value.at("nonce").as_int64()));
    } catch (const boost::exception &o) {
        std::cout << ec.message() << std::endl;
        return std::nullopt;
    }
}

std::string WalletAccount::serialize() const {
    std::stringstream ss;
    std::stringstream balanceSS;
    balanceSS << std::hex << this->balance;
    ss << R"({"address":")" << this->address << R"(", "balance":")" << balanceSS.str() << R"(", "nonce":)" << this->nonce << ((tokensBalance.empty()) ? "}" : ", ");
    if (!tokensBalance.empty()) ss << R"("tokensBalance":)" << boost::json::serialize(this->tokensBalance) << "}";
    return ss.str();
}

void WalletAccount::increase(const uint256_t& value, const std::string& inputHash) {
    this->balance += value;
    this->txInputs.emplace_back(inputHash);
    this->changed = true;
}

operationStatus::WalletErrorsCode WalletAccount::subtract(const uint256_t& value, const std::string& outputHash) {
    if (this->balance < value) return operationStatus::WalletErrorsCode::cLowNativeTokenBalance;
    this->balance -= value;
    this->txOutputs.emplace_back(outputHash);
    this->changed = true;
    return operationStatus::WalletErrorsCode::cOk;
}

operationStatus::WalletErrorsCode WalletAccount::subtractToken(const uint256_t& value, const std::string &inputHash, const std::string &tokenName) {
    if (!this->tokensBalance.contains(tokenName)) return operationStatus::WalletErrorsCode::cTokenBalanceNotFound;
    if (uint256_t(boost::json::value_to<std::string>(this->tokensBalance[tokenName])) < value) return operationStatus::WalletErrorsCode::cLowTokenBalance;
    this->tokensBalance[tokenName] = WalletAccount::uint256_diff_2string(uint256_t(boost::json::value_to<std::string>(this->tokensBalance[tokenName])), value); // (a - b).toString();
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

void WalletAccount::increaseToken(const uint256_t& value, const std::string &inputHash, const std::string &tokenName) {
    if (!this->tokensBalance.contains(tokenName)) {
        this->tokensBalance[tokenName] = WalletAccount::uint256_2string(value);
        this->changed = true;
        return;
    }
    this->tokensBalance[tokenName] = WalletAccount::uint256_sum_2string(uint256_t(boost::json::value_to<std::string>(this->tokensBalance[tokenName])), value);
    this->changed = true;
}

WalletAccount::WalletAccount(std::string address, uint256_t balance, json::object tokensBalance) : address(std::move(
        address)), balance(std::move(balance)), tokensBalance(std::move(tokensBalance)) {}

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
        std::cout << ec.message() << std::endl;
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

WalletAccount::WalletAccount(std::string address, uint256_t balance, json::object tokensBalance, uint64_t nonce) : address(std::move(
        address)), balance(std::move(balance)), tokensBalance(std::move(tokensBalance)), nonce(nonce) {}

bool WalletAccount::isValidNonce(RawTransaction *rawPointer) const {
    return this->nonce != rawPointer->nonce;
}

WalletAccount* WalletAccount::createEmptyWallet(const std::string &address) {
    return std::make_shared<WalletAccount>(WalletAccount()).get();
}

int WalletAccount::compareNativeTokenBalance(const json::value &amount) const {
    uint256_t amountOfTokens = uint256_t(boost::json::value_to<std::string>(amount));
    return (this->balance < amountOfTokens) ? -1 : (this->balance > amountOfTokens) ? 1 : 0;
}

int WalletAccount::compareTokenBalance(const json::value &amount, const json::value &tokenName) {
    uint256_t amountOfTokens = uint256_t(boost::json::value_to<std::string>(amount));
    uint256_t amountOfTokensInBalance = uint256_t(boost::json::value_to<std::string>(this->tokensBalance[boost::json::value_to<std::string>(tokenName)]));
    return (amountOfTokensInBalance < amountOfTokens) ? -1 : (amountOfTokensInBalance > amountOfTokens) ? 1 : 0;
}


#endif //UNIT_WALLETACCOUNT_H
