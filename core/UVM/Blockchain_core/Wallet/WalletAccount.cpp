//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "WalletAccount.h"

const std::string &WalletAccount::getAddress() const {
    return address;
}

void WalletAccount::setAddress(const std::string &address) {
    WalletAccount::address = address;
}

double WalletAccount::getAmount() const {
    return amount;
}

void WalletAccount::setAmount(double amount) {
    WalletAccount::amount = amount;
}

const std::map<std::string, double> &WalletAccount::getNonDefaultBalances() const {
    return non_default_balances;
}

void WalletAccount::setNonDefaultBalances(const std::map<std::string, double> &nonDefaultBalances) {
    non_default_balances = nonDefaultBalances;
}

std::ostream &operator<<(std::ostream &out, const WalletAccount &walletAccount) {
    std::map<std::string, double> extra_map = walletAccount.non_default_balances;
    std::vector<std::string> key;
    std::vector<double> value;
    for (auto & it : extra_map) {
        key.push_back(it.first);
        value.push_back(it.second);
    }
    std::string serialized_token_balances;
    for (int i = 0; i < key.size(); i++) {
        serialized_token_balances
        .append("{\"")
        .append(key[i])
        .append("\":")
        .append(std::to_string(value[i]))
        .append((i == key.size() - 1) ? "}" : "},");
    }

    return out << R"({"address":")" << walletAccount.address <<  R"(", "amount":")" << walletAccount.amount <<  R"(", "tokens_balance":[)" << serialized_token_balances << "]}";
}


std::string WalletAccount::to_json_string() {
    return std::string();
}

bool WalletAccount::operator==(const WalletAccount &rhs) const {
    return address == rhs.address;
}

bool WalletAccount::operator!=(const WalletAccount &rhs) const {
    return !(rhs == *this);
}
