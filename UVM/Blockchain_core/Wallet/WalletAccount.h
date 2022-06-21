//
// Created by Kirill Zhukov on 12.06.2022.
//

#ifndef UVM_WALLETACCOUNT_H
#define UVM_WALLETACCOUNT_H
#include "string"
#include "iostream"
#include "map"
#include "sstream"
#include "vector"
#include <nlohmann/json.hpp>

class WalletAccount {
public:
    WalletAccount();

    WalletAccount(const std::string &address, double amount,
                  const std::map<std::string, std::string> &nonDefaultBalances);

    std::string address;
    double amount;
    std::map<std::string, std::string> non_default_balances;

    // getters & setters
    [[nodiscard]] const std::string &getAddress() const;
    void setAddress(const std::string &address);
    [[nodiscard]] double getAmount() const;
    void setAmount(double amount);
    [[nodiscard]] const std::map<std::string, std::string> &getNonDefaultBalances() const;
    void setNonDefaultBalances(const std::map<std::string, std::string> &nonDefaultBalances);

    //bool operators
    bool operator==(const WalletAccount &rhs) const;
    bool operator!=(const WalletAccount &rhs) const;

    friend std::ostream& operator<< (std::ostream &out, const WalletAccount &transaction);

    std::string to_json_string();
    void serialize_from_json(std::string &account);
    static nlohmann::json subtract_unit_balance(nlohmann::json &account, double value);
    static nlohmann::json increase_unit_balance(nlohmann::json &account, double value);
};


#endif //UVM_WALLETACCOUNT_H
