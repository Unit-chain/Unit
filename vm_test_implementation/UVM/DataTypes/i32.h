//
// Created by Kirill Zhukov on 07.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_I32_H
#define VM_TEST_IMPLEMENTATION_I32_H
#include "Object.h"
#define TYPE 1


class i32 : public Object {
public:
    inline i32() = default;

    inline i32(i32 &u) : Object(u, TYPE) {
        *this = u;
    }

    inline explicit i32(int u) : Object(std::to_string(u), TYPE) {
        this->value = u;
    }

    inline explicit i32(const std::string &str) {
        std::istringstream iss(str);
        iss >> this->value;
        this->type = TYPE;
        this->object = new std::string(str);
    }

    int value{};

    [[nodiscard]] inline const int &getValue() const {
        return value;
    }

    inline void setValue(const int &val) {
        i32::value = val;
    }

    inline bool operator==(const i32 &rhs) const {
        return this->value == rhs.value;
    }

    inline bool operator==(const Object &rhs) const {
        return this->value == ((i32) rhs).value;
    }

    inline bool operator!=(const i32 &rhs) const {
        return !(rhs == *this);
    }

    inline bool operator!=(const Object &rhs) const {
        return !(((i32) rhs) == *this);
    }

    inline bool operator<(const i32 &rhs) const {
        return value < rhs.value;
    }

    inline bool operator<(const Object &rhs) const {
        return value < ((i32) rhs).value;
    }

    inline bool operator>(const i32 &rhs) const {
        return rhs < *this;
    }

    inline bool operator>(const Object &rhs) const {
        return ((i32) rhs) < *this;
    }

    inline bool operator<=(const i32 &rhs) const {
        return !(rhs < *this);
    }

    inline bool operator<=(const Object &rhs) const {
        return !(((i32) rhs) < *this);
    }

    inline bool operator>=(const i32 &rhs) const {
        return !(*this < rhs);
    }

    inline bool operator>=(const Object &rhs) const {
        return !(*this < ((i32) rhs));
    }

    inline i32 operator+(const i32 &rhs) const {
        return i32(this->value + rhs.value);
    }

    inline i32 operator||(const i32 &rhs) const {
        return i32(this->value || rhs.value);
    }

    inline i32 operator&&(const i32 &rhs) const {
        return i32(this->value && rhs.value);
    }

    inline i32 operator+(const Object &rhs) const {
        return i32(this->value + ((i32) rhs).value);
    }

    inline i32 operator-(const i32 &rhs) const {
        if (*this < rhs)
            return i32(0);
        return i32(this->value - rhs.value);
    }

    inline i32 operator-(const Object &rhs) const {
        if (*this < ((i32) rhs))
            return i32(0);
        return i32(this->value - ((i32) rhs).value);
    }

    inline i32 &operator+=(i32 c2) {
        this->value += c2.value;
        return *this;
    }

    inline i32 &operator+=(Object &c2) {
        this->value += ((i32) c2).value;
        return *this;
    }

    inline i32 &operator-=(i32 c2) {
        if (*this < c2) {
            i32 c1 = i32(0);
            return c1;
        }
        this->value -= c2.value;
        return *this;
    }

    inline i32 &operator-=(Object &c2) {
        if (*this < ((i32) c2)) {
            i32 c1 = i32(0);
            return c1;
        }
        this->value -= ((i32) c2).value;
        return *this;
    }

    // prefix
    inline i32 &operator++() {
        value += 1;
        return *this;
    }

    inline i32 &operator!() {
        !value;
        return *this;
    }

    inline i32 &operator--() {
        value -= 1;
        return *this;
    }

    // postfix
    inline i32 operator++(int) {
        ++(this->value);
        return *this;
    }

    inline i32 operator--(int) {
        --(this->value);
        return *this;
    }

    inline i32 operator*(const i32 &rhs) const {
        return i32(this->value * rhs.value);
    }

    inline i32 operator*(const Object &rhs) const {
        return i32(this->value * ((i32) rhs).value);
    }

    inline i32 operator/(const i32 &rhs) const {
        return i32(this->value / rhs.value);
    }

    inline i32 operator/(const Object &rhs) const {
        return i32(this->value / ((i32) rhs).value);
    }

    inline i32 operator%(const i32 &rhs) const {
        return i32(this->value % rhs.value);
    }

    inline i32 operator%(const Object &rhs) const {
        return i32(this->value % ((i32) rhs).value);
    }

    inline i32 operator^(const i32 &rhs) const {
        return i32(this->value ^ rhs.value);
    }

    inline i32 operator^(const Object &rhs) const {
        return i32(this->value ^ ((i32) rhs).value);
    }

    inline i32 operator&(const i32 &rhs) const {
        return i32(this->value & rhs.value);
    }

    inline i32 operator&(const Object &rhs) const {
        return i32(this->value & ((i32) rhs).value);
    }

    inline i32 operator<<(const int num) const {
        return i32(value << num);
    }

    inline i32 operator<<(const i32 num) const {
        return i32(value << num.value);
    }

    inline i32 operator>>(const i32 num) const {
        return i32(value >> num.value);
    }

    inline i32 operator>>(const int num) const {
        return i32(value >> num);
    }

    inline i32 operator<<=(const int num) {
        value <<= num;
        return *this;
    }

    inline i32 operator>>=(const int num)  {
        value >>= num;
        return *this;
    }

    inline i32 operator<<=(const i32 num) {
        value <<= num.value;
        return *this;
    }

    inline i32 operator>>=(const i32 num)  {
        value >>= num.value;
        return *this;
    }

    inline i32 &operator*=(i32 c2) {
        this->value *= c2.value;
        return *this;
    }

    inline i32 &operator*=(Object c2) {
        this->value *= ((i32) c2).value;
        return *this;
    }

    inline i32 &operator/=(i32 c2) {
        this->value /= c2.value;
        return *this;
    }

    inline i32 &operator/=(Object c2) {
        this->value /= ((i32) c2).value;
        return *this;
    }

    inline i32 &operator%=(i32 c2) {
        this->value %= c2.value;
        return *this;
    }

    inline i32 &operator%=(Object c2) {
        this->value %= ((i32) c2).value;
        return *this;
    }

    inline i32 &operator^=(i32 c2) {
        this->value ^= c2.value;
        return *this;
    }

    inline i32 &operator^=(Object c2) {
        this->value ^= ((i32) c2).value;
        return *this;
    }

    inline i32 &operator&=(i32 &c2) {
        this->value &= c2.value;
        return *this;
    }

    inline i32 &operator&=(Object &c2) {
        this->value &= ((i32) c2).value;
        return *this;
    }

    inline i32 &operator=(const i32 &c2) {
        this->value = c2.value;
        return *this;
    }

    inline i32 &operator=(Object &c2) {
        this->value = ((i32) c2).value;
        return *this;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const i32 &i32) {
        return os << R"({"value":)" << i32.value << R"(,"type":)" << TYPE << "}";
    }

    // implicit conversion
    inline operator Object() const {
        return Object(std::to_string(this->value));
    }

    // explicit conversion
    inline explicit operator Object *() const {
        return new Object(std::to_string(this->value));
    }

    inline virtual std::string toString() override {
        return std::to_string(this->value);
    }
};
#endif //VM_TEST_IMPLEMENTATION_I32_H
