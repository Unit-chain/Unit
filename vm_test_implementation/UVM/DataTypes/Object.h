//
// Created by Kirill Zhukov on 03.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_OBJECT_H
#define VM_TEST_IMPLEMENTATION_OBJECT_H
#include "iostream"
#include <boost/json/src.hpp>
#include <utility>
#include "boost/json/object.hpp"


class Object {
public:

    inline Object() = default;

    inline explicit Object(std::string value, unsigned char type = 0) {
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
        this->object = new std::string(value);
        this->type = type;
    }

    inline explicit Object(std::string &value, unsigned char type = 0) {
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
        this->object = new std::string(value);
        this->type = type;
    }

    virtual ~Object() {
        this->object = nullptr;
        delete(this->object);
    }

    std::string *object = nullptr;
    uint16_t type;

    [[nodiscard]] virtual inline std::optional<std::string> get(std::string key) const {
        boost::json::object obj = boost::json::parse(*this->object).as_object();
        if(obj.contains(key))
            try {
                return boost::json::value_to<std::string>(obj.at(key));
            } catch (std::exception &e){
                return std::nullopt;
            }
        else
            return std::nullopt;
    }

    inline void setObject(std::string str) {
        this->object = new std::string(std::move(str));
    }

    [[nodiscard]] inline bool isNull() const { return this->object == nullptr; }

    inline virtual std::string toString() {
        return *this->object;
    }

    inline virtual bool equals(Object &object1) {
        return *this->object == *object1.object;
    }

    inline bool operator<(const Object &rhs) const {
        return *this->object < *rhs.object;
    }

    inline Object &operator=(const Object &c2) {
        this->object = new std::string(*c2.object);
        this->type = c2.type;
        return *this;
    }

    inline bool operator>(const Object &rhs) const {
        return rhs < *this;
    }

    inline bool operator<=(const Object &rhs) const {
        return *this->object <= *rhs.object;
    }

    inline bool operator>=(const Object &rhs) const {
        return rhs <= *this;
    }

    inline bool operator==(const Object &rhs) const {
        return *this->object == *rhs.object;
    }

    inline bool operator!=(const Object &rhs) const {
        return *this->object != *rhs.object;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const Object &obj) {
        return os << R"({"value":)" << R"(")" << ((obj.object != nullptr) ? *obj.object : "") << R"(")" << R"(,"type":)" << obj.type << "}";
    }

    // implicit conversion
    template<class T>
    inline operator T() const {
        return T(*this->object);
    }

    // explicit conversion
    template<class T>
    inline explicit operator Object *() const {
        return new T(*this->object);
    }
};


#endif //VM_TEST_IMPLEMENTATION_OBJECT_H