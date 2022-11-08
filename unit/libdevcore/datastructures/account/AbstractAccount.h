//
// Created by Kirill Zhukov on 08.11.2022.
//

#ifndef UNIT_ABSTRACTACCOUNT_H
#define UNIT_ABSTRACTACCOUNT_H
#include "iostream"
#include "strstream"
#include "boost/multiprecision/cpp_int.hpp"

using namespace boost::multiprecision;


/// abstract class for defining methods
class AbstractAccount {
public:
    inline static std::string uint256_2string(const uint256_t& value) {
        std::stringstream balanceSS;
        balanceSS << std::hex << value;
        return balanceSS.str();
    }
    inline static std::string uint256_sum_2string(const uint256_t& value1, const uint256_t& value2) {
        std::stringstream balanceSS;
        balanceSS << std::hex << (value1 + value2);
        return balanceSS.str();
    }
    inline static std::string uint256_diff_2string(const uint256_t& value1, const uint256_t& value2) {
        std::stringstream balanceSS;
        balanceSS << std::hex << (value1 - value2);
        return balanceSS.str();
    }
};
#endif //UNIT_ABSTRACTACCOUNT_H
