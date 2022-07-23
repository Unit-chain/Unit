//
// Created by Kirill Zhukov on 07.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_U128_H
#define VM_TEST_IMPLEMENTATION_U128_H

#include <utility>
#include <ostream>
#include "Object.h"
#include "boost/multiprecision/cpp_int.hpp"
#define TYPE 3


class u128 : public Object {
public:
    inline u128() = default;

    inline u128(u128 &u) : Object(u, TYPE) {
        *this = u;
    }

    inline explicit u128(int u) : Object(std::to_string(u), TYPE) {
        this->value = u;
    }

    inline explicit u128(uint64_t &u) : Object(std::to_string(u), TYPE) {
        this->value = u;
    }

    inline explicit u128(boost::multiprecision::uint128_t u) : Object(u.convert_to<std::string>(), TYPE) {
        this->value = std::move(u);
    }

    inline explicit u128(const std::string& str) : Object(str, TYPE) {
        std::string intVal = (((str.find('.') == std::string::npos)) ? str : str.substr(0, str.find('.')));
        this->value = boost::multiprecision::uint128_t(intVal);
        this->type = TYPE;
    }

    boost::multiprecision::uint128_t value;

    [[nodiscard]] inline const boost::multiprecision::uint128_t &getValue() const {
        return value;
    }

    inline void setValue(const boost::multiprecision::uint128_t &val) {
        u128::value = val;
    }

    inline bool operator==(const u128 &rhs) const {
        return this->value == rhs.value;
    }

    inline bool operator==(const Object &rhs) const {
        return this->value == ((u128)rhs).value;
    }

    inline bool operator!=(const u128 &rhs) const {
        return !(rhs == *this);
    }

    inline bool operator!=(const Object &rhs) const {
        return !((u128) rhs == *this);
    }

    inline bool operator<(const u128 &rhs) const {
        return value < rhs.value;
    }

    inline bool operator<(const Object &rhs) const {
        return value < ((u128)rhs).value;
    }

    inline bool operator>(const u128 &rhs) const {
        return rhs < *this;
    }

    inline bool operator>(const Object &rhs) const {
        return (u128) rhs < *this;
    }

    inline bool operator<=(const u128 &rhs) const {
        return !(rhs < *this);
    }

    inline bool operator<=(const Object &rhs) const {
        return !((u128) rhs < *this);
    }

    inline bool operator>=(const u128 &rhs) const {
        return !(*this < rhs);
    }

    inline bool operator>=(const Object &rhs) const {
        return !(*this < (u128) rhs);
    }

    inline u128 operator+(const u128 &rhs) const {
        return u128(this->value + rhs.value);
    }

    inline u128 operator+(const Object &rhs) const {
        return u128(this->value + ((u128) rhs).value);
    }

    inline u128 operator||(const u128 &rhs) const {
        return u128(this->value || rhs.value);
    }

    inline u128 operator&&(const u128 &rhs) const {
        return u128(this->value && rhs.value);
    }

    inline u128 operator-(const u128 &rhs) const {
        if (*this < rhs)
            return u128(0);
        return u128(this->value - rhs.value);
    }

    inline u128 operator-(const Object &rhs) const {
        if (*this < rhs)
            return u128(0);
        return u128(this->value - ((u128) rhs).value);
    }

    inline u128 &operator+=(u128 c2) {
        this->value += c2.value;
        this->object = new std::string(c2.value.convert_to<std::string>());
        return *this;
    }

    inline u128 &operator+=(Object c2) {
        this->value += ((u128) c2).value;
        return *this;
    }

    inline u128 &operator-=(u128 c2) {
        if (*this < c2) {
            u128 c1 = u128(0);
            return c1;
        }
        this->value -= c2.value;
        return *this;
    }

    inline u128 &operator-=(Object &c2) {
        if (*this < ((u128) c2)) {
            u128 c1 = u128(0);
            return c1;
        }
        this->value -= ((u128) c2).value;
        return *this;
    }

    // prefix
    inline u128 &operator++() {
        value += 1;
        return *this;
    }

    inline u128 &operator--() {
        value -= 1;
        return *this;
    }

    // postfix
    inline u128 operator++(int) {
        ++(this->value);
        return *this;
    }

    inline u128 operator--(int) {
        --(this->value);
        return *this;
    }

    inline u128 &operator!() {
        !value;
        return *this;
    }

    inline u128 operator*(const u128 &rhs) const {
        return u128(this->value * rhs.value);
    }

    inline u128 operator*(const Object &rhs) const {
        return u128(this->value * ((u128) rhs).value);
    }

    inline u128 operator/(const u128 &rhs) const {
        return u128(this->value / rhs.value);
    }

    inline u128 operator/(const Object &rhs) const {
        return u128(this->value / ((u128) rhs).value);
    }

    inline u128 operator%(const u128 &rhs) const {
        return u128(this->value % rhs.value);
    }

    inline u128 operator%(const Object &rhs) const {
        return u128(this->value % ((u128) rhs).value);
    }

    inline u128 operator^(const u128 &rhs) const {
        return u128(this->value ^ rhs.value);
    }

    inline u128 operator^(const Object &rhs) const {
        return u128(this->value ^ ((u128) rhs).value);
    }

    inline u128 operator&(const u128 &rhs) const {
        return u128(this->value & rhs.value);
    }

    inline u128 operator&(const Object &rhs) const {
        return u128(this->value & ((u128) rhs).value);
    }

    inline u128 operator<<(const int num) const {
        return u128(value << num);
    }

    inline u128 operator>>(const int num) const {
        return u128(value >> num);
    }

    inline u128 operator<<=(const int num) {
        value <<= num;
        return *this;
    }

    inline u128 operator>>=(const int num)  {
        value >>= num;
        return *this;
    }

    inline u128 &operator*=(u128 c2) {
        this->value *= c2.value;
        return *this;
    }

    inline u128 &operator*=(Object c2) {
        this->value *= ((u128) c2).value;
        return *this;
    }

    inline u128 &operator/=(u128 c2) {
        this->value /= c2.value;
        return *this;
    }

    inline u128 &operator/=(Object c2) {
        this->value /= ((u128) c2).value;
        return *this;
    }

    inline u128 &operator%=(u128 c2) {
        this->value %= c2.value;
        return *this;
    }

    inline u128 &operator%=(Object c2) {
        this->value %= ((u128) c2).value;
        return *this;
    }

    inline u128 &operator^=(u128 c2) {
        this->value ^= c2.value;
        return *this;
    }

    inline u128 &operator^=(Object c2) {
        this->value ^= ((u128) c2).value;
        return *this;
    }

    inline u128 &operator&=(u128 &c2) {
        this->value &= c2.value;
        return *this;
    }

    inline u128 &operator&=(Object &c2) {
        this->value &= ((u128) c2).value;
        return *this;
    }

    inline u128 &operator=(const u128 &c2) {
        this->value = c2.value;
        return *this;
    }

    inline u128 &operator=(Object &c2) {
        this->value = ((u128) c2).value;
        return *this;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const u128 &u128) {
        return os << R"({"value":)" << u128.value << R"(,"type":)" << TYPE << "}";
    }

    // implicit conversion
    inline operator Object() const {
        return Object(this->value.str());
    }

    // explicit conversion
    inline explicit operator Object *() const {
        return new Object(this->value.str());
    }

    inline virtual std::string toString() override {
        return this->value.str();
    }
};

#endif //VM_TEST_IMPLEMENTATION_U128_H