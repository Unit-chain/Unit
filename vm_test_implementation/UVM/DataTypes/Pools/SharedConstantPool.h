//
// Created by Kirill Zhukov on 11.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_SHAREDCONSTANTPOOL_H
#define VM_TEST_IMPLEMENTATION_SHAREDCONSTANTPOOL_H
#include "boost/multiprecision/cpp_int.hpp"

class SharedConstantPool {
public:
    boost::multiprecision::uint256_t MAX_VALUE = boost::multiprecision::uint256_t(1.1579209e+77);
};

#endif //VM_TEST_IMPLEMENTATION_SHAREDCONSTANTPOOL_H
