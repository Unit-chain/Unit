//
// Created by Kirill Zhukov on 07.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_BOOLEAN_H
#define VM_TEST_IMPLEMENTATION_BOOLEAN_H
#include "Object.h"
#include <boost/lexical_cast.hpp>
#define TYPE 6


class boolean : public Object{
public:
    inline boolean() = default;

    inline explicit boolean(bool value) : Object((value) ? "true" : "false", TYPE){
        this->value = value;
    }

    inline explicit boolean(const std::string &str) : Object(str, TYPE){ // : Object(std::move(str))
        try {
            this->value = boost::lexical_cast<bool>(str);
        } catch (boost::bad_lexical_cast::bad_lexical_cast::bad_cast &) {
            this->value = (str == "true");
        }
        this->type = TYPE;
    }

    inline explicit boolean(int value) : Object((value) ? "true" : "false", 6){
        this->value = value;
    }

    bool value{};

    inline bool operator==(const boolean &rhs) const {
        return this->value == rhs.value;
    }

    inline bool &operator=(const bool &rhs) {
        return this->value = rhs;
    }

    inline bool operator!=(const boolean &rhs) const {
        return !(rhs == *this);
    }

    inline friend std::ostream &operator<<(std::ostream &os, const boolean &boolean1) {
        return os << R"({"value":)" << boolean1.value << R"(,"type":)" << TYPE << "}";
    }

    // implicit conversion
    inline operator Object() const {
        return Object((this->value) ? "true" : "false");
    }

    // explicit conversion
    inline explicit operator Object *() const {
        return new Object((this->value) ? "true" : "false");
    }
};

#endif //VM_TEST_IMPLEMENTATION_BOOLEAN_H
