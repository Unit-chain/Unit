//
// Created by Kirill Zhukov on 07.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_U64_H
#define VM_TEST_IMPLEMENTATION_U64_H
#include "Object.h"

class u64 : public Object {
public:
    inline u64() = default;

    inline u64(u64 &u) : Object(u, 2) {
        *this = u;
    }

    inline explicit u64(int u) : Object(std::to_string(u), 2) {
        this->value = u;
    }

    inline explicit u64(uint64_t u) : Object(std::to_string(u), 2) {
        this->value = u;
    }

    inline explicit u64(const std::string &str) {
        std::istringstream iss(str);
        iss >> this->value;
        this->type = TYPE;
    }

    uint64_t value{};

    [[nodiscard]] inline const uint64_t &getValue() const {
        return value;
    }

    inline void setValue(const uint64_t &val) {
        u64::value = val;
    }

    inline bool operator==(const u64 &rhs) const {
        return this->value == rhs.value;
    }

    inline bool operator==(const Object &rhs) const {
        return this->value == ((u64) rhs).value;
    }

    inline bool operator!=(const u64 &rhs) const {
        return !(rhs == *this);
    }

    inline bool operator!=(const Object &rhs) const {
        return !(((u64) rhs) == *this);
    }

    inline bool operator<(const u64 &rhs) const {
        return value < rhs.value;
    }

    inline bool operator<(const Object &rhs) const {
        return value < ((u64) rhs).value;
    }

    inline bool operator>(const u64 &rhs) const {
        return rhs < *this;
    }

    inline bool operator>(const Object &rhs) const {
        return ((u64) rhs) < *this;
    }

    inline bool operator<=(const u64 &rhs) const {
        return !(rhs < *this);
    }

    inline bool operator<=(const Object &rhs) const {
        return !(((u64) rhs) < *this);
    }

    inline bool operator>=(const u64 &rhs) const {
        return !(*this < rhs);
    }

    inline bool operator>=(const Object &rhs) const {
        return !(*this < ((u64) rhs));
    }

    inline uint64_t operator+(const u64 rhs) const {
        return (this->value + rhs.value);
    }

    inline uint64_t operator+(const Object rhs) const {
        return (this->value + ((u64) rhs).value);
    }

    inline u64 operator||(const u64 &rhs) const {
        return u64(this->value || rhs.value);
    }

    inline u64 operator&&(const u64 &rhs) const {
        return u64(this->value && rhs.value);
    }

    inline uint64_t operator-(const u64 &rhs) const {
        if (*this < rhs)
            return 0;
        return (this->value - rhs.value);
    }

    inline uint64_t operator-(const Object &rhs) const {
        if (*this < ((u64) rhs))
            return 0;
        return (this->value - ((u64) rhs).value);
    }

    inline u64 &operator+=(u64 &c2) {
        this->value += c2.value;
        return *this;
    }

    inline u64 &operator+=(Object c2) {
        this->value += ((u64) c2).value;
        return *this;
    }

    inline u64 &operator-=(u64 c2) {
        if (*this < c2) {
            u64 c1 = u64(0);
            return c1;
        }
        this->value -= c2.value;
        return *this;
    }

    inline u64 &operator-=(Object &c2) {
        if (*this < ((u64) c2)) {
            u64 c1 = u64(0);
            return c1;
        }
        this->value -= ((u64) c2).value;
        return *this;
    }

    // prefix
    inline u64 &operator++() {
        value += 1;
        return *this;
    }

    inline u64 &operator--() {
        value -= 1;
        return *this;
    }

    inline u64 &operator!() {
        !value;
        return *this;
    }

    // postfix
    inline u64 operator++(int) {
        ++(this->value);
        return *this;
    }

    inline u64 operator--(int) {
        --(this->value);
        return *this;
    }

    inline u64 operator*(const u64 &rhs) const {
        return u64(this->value * rhs.value);
    }

    inline u64 operator*(const Object &rhs) const {
        return u64(this->value * ((u64) rhs).value);
    }

    inline u64 operator/(const u64 &rhs) const {
        return u64(this->value / rhs.value);
    }

    inline u64 operator/(const Object &rhs) const {
        return u64(this->value / ((u64) rhs).value);
    }

    inline u64 operator%(const u64 &rhs) const {
        return u64(this->value % rhs.value);
    }

    inline u64 operator%(const Object &rhs) const {
        return u64(this->value % ((u64) rhs).value);
    }

    inline u64 operator^(const u64 &rhs) const {
        return u64(this->value ^ rhs.value);
    }

    inline u64 operator^(const Object &rhs) const {
        return u64(this->value ^ ((u64) rhs).value);
    }

    inline u64 operator&(const u64 &rhs) const {
        return u64(this->value & rhs.value);
    }

    inline u64 operator&(const Object &rhs) const {
        return u64(this->value & ((u64) rhs).value);
    }

    inline u64 operator<<(const int num) const {
        return u64(value << num);
    }

    inline u64 operator<<(const i32 num) const {
        return u64(value << num.value);
    }

    inline u64 operator>>(const i32 num) const {
        return u64(value >> num.value);
    }

    inline u64 operator>>(const int num) const {
        return u64(value >> num);
    }

    inline u64 operator<<=(const int num) {
        value <<= num;
        return *this;
    }

    inline u64 operator>>=(const int num)  {
        value >>= num;
        return *this;
    }

    inline u64 operator<<=(const u64 num) {
        value <<= num.value;
        return *this;
    }

    inline u64 operator>>=(const u64 num)  {
        value >>= num.value;
        return *this;
    }

    inline u64 &operator*=(u64 c2) {
        this->value *= c2.value;
        return *this;
    }

    inline u64 &operator*=(Object c2) {
        this->value *= ((u64) c2).value;
        return *this;
    }

    inline u64 &operator/=(u64 c2) {
        this->value /= c2.value;
        return *this;
    }

    inline u64 &operator/=(Object c2) {
        this->value /= ((u64) c2).value;
        return *this;
    }

    inline u64 &operator%=(u64 c2) {
        this->value %= c2.value;
        return *this;
    }

    inline u64 &operator%=(Object c2) {
        this->value %= ((u64) c2).value;
        return *this;
    }

    inline u64 &operator^=(u64 c2) {
        this->value ^= c2.value;
        return *this;
    }

    inline u64 &operator^=(Object c2) {
        this->value ^= ((u64) c2).value;
        return *this;
    }

    inline u64 &operator&=(u64 &c2) {
        this->value &= c2.value;
        return *this;
    }

    inline u64 &operator&=(Object &c2) {
        this->value &= ((u64) c2).value;
        return *this;
    }

    inline u64 &operator=(const u64 c2) {
        this->value = c2.value;
        return *this;
    }

    inline u64 &operator=(Object &c2) {
        this->value = ((u64) c2).value;
        return *this;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const u64 &u64) {
        os << u64.value;
        return os;
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
#endif //VM_TEST_IMPLEMENTATION_U64_H
