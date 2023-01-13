//
// Created by Kirill Zhukov on 04.11.2022.
//

#ifndef UNIT_TOKEN_H
#define UNIT_TOKEN_H
#include <utility>

#include "iostream"
#include "optional"
#include "sstream"
#include "boost/json.hpp"
#include "boost/json/src.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "GlobalVariables.h"
#include "AbstractAccount.h"

using namespace boost::multiprecision;

/// apologies to any of you who are Ethereum's fans
/// this was made in a hurry :(
inline std::string string_to_hex(std::string &value_to_hex) {
    std::stringstream stream;
    for (const auto &item : value_to_hex) {
        stream << std::hex << int(item);
    }
    return stream.str();
}

inline std::string bytes_to_hex(uint8_t value_to_hex[32]) {
    std::stringstream stream;
    for (int i = 0; i < 32; i++) {
        stream << std::hex << std::setfill('0') << std::setw(2) << unsigned(value_to_hex[i]);
    }
    return stream.str();
}

inline std::string hex_to_ascii(std::string &hex){
    int len = hex.length();
    std::string newString;
    for(int i = 0; i < len; i += 2) {
        std::string byte = hex.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), nullptr, 16);
        newString.push_back(chr);
    }
    return newString;
}

class Token : public AbstractAccount {
public:
    Token(std::string name, std::string bytecode, const std::string& supply) : name(std::move(name)), bytecode(std::move(bytecode)),
                                                                                 supply(supply) {}

    std::string name;
    std::string bytecode;
    uint256_t supply;
    static std::optional<std::shared_ptr<Token>> parse(boost::json::value *extra);
    [[nodiscard]] std::string serialize() const;
};

std::optional<std::shared_ptr<Token>> Token::parse(boost::json::value *extra) {
    boost::json::value parsedData{};
    try {
        auto bytecode = boost::json::value_to<std::string>(extra->at("bytecode"));
        parsedData = boost::json::parse(hex_to_ascii(bytecode));
        Token newToken = Token(boost::json::value_to<std::string>(parsedData.at("name")), bytecode, boost::json::value_to<std::string>(parsedData.at("supply")));
        return {std::make_shared<Token>(newToken)};
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return std::nullopt;
    }
}

std::string Token::serialize() const {
    std::stringstream ss;
    ss << R"({"name":")" << this->name << R"(","bytecode":")" << this->bytecode << R"(", "supply":")" << Token::uint256_2string(this->supply) << R"("})";
    return ss.str();
}

#endif //UNIT_TOKEN_H
