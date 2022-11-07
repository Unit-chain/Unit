//
// Created by Kirill Zhukov on 11.06.2022.
//

#ifndef UVM_HEX_H
#define UVM_HEX_H
#include "iostream"
#include "iomanip"
#include <sstream>
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
#endif //UVM_HEX_H
