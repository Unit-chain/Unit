//
// Created by Kirill Zhukov on 07.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_COLLECTION_H
#define VM_TEST_IMPLEMENTATION_COLLECTION_H

#include <ostream>
#include "Object.h"
#include <boost/json/src.hpp>
#include <utility>
#include <boost/json/value.hpp>
#include <boost/json/value_to.hpp>
#include "boost/json/value_from.hpp"
#include "i32.h"
#include "u64.h"
#include "u128.h"
#include "u256.h"
#include "d64.h"
#include "str.h"
#include "boolean.h"
#include "vector"
#include "algorithm"
#define TYPE 7


class collection : public Object{
public:
    collection() = default;
    std::vector<Object> value;

    inline explicit collection(std::vector<Object> coll) {
        this->value = std::move(coll);
        *this->object = this->serializeValues();
        this->type = TYPE;
    }

    inline explicit collection(const std::string &coll) {
        this->deserialize(coll);
        *this->object = coll;
        this->type = TYPE;
    }

    inline void emplace_back(const Object& obj) {
        value.emplace_back(obj);
    }

    inline void push_back(const Object& obj) {
        value.push_back(obj);
    }

    inline void pop_back() {
        return value.pop_back();
    }

    inline int size() const{
        return this->value.size();
    }

    inline unsigned long find(int start, int end, Object &key) {
        return std::binary_search(this->value.begin(), this->value.end(), key);
    }

    template<class T>
    inline T forEach(std::function<T> func) {
        func(this->value);
    }

    template<class T>
    inline T getWithCast(int index){
        return (T) this->value[index];
    }

    inline Object& operator[](int index) {
        return this->value[index];
    }

    friend std::ostream &operator<<(std::ostream &os, collection collection) {
        return os << R"({"value":)" << collection.serializeValues() << R"(,"type":)" << TYPE << "}";
    }

    inline explicit operator Object() {
        return Object(this->serializeValues());
    }

    // explicit conversion
    inline explicit operator Object *() {
        return new Object(this->serializeValues());
    }

    inline std::string serializeValues() {
        std::stringstream stringstream;
        stringstream << "[";
        for(int i = 0; i < value.size(); i++) {
            switch (value[i].type) {
                case 1:
                    stringstream << (i32) value[i];
                    break;
                case 2:
                    stringstream << (u64) value[i];
                    break;
                case 3:
                    stringstream << (u128) value[i];
                    break;
                case 4:
                    stringstream << (u256) value[i];
                    break;
                case 5:
                    stringstream << (d64) value[i];
                    break;
                case 6:
                    stringstream << (boolean) value[i];
                    break;
                case 7:
                    stringstream << (collection) value[i];
                    break;
                case 8:
                    stringstream << (str) value[i];
                    break;
                default:
                    stringstream << (Object) value[i];
                    break;
            }
            (i != value.size() - 1) ? stringstream << ',' : stringstream << ']';
        }
        return stringstream.str();
    }

    inline void deserialize(const std::string &unique_values) {
        boost::json::value jv = boost::json::parse(unique_values);
        for (int i = 0; i < jv.as_array().size(); i++) {
            switch (jv.as_array()[i].as_object()["type"].as_int64()) {
                case 0:
                    this->value.emplace_back(Object(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 1:
                    this->value.emplace_back(i32(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 2:
                    this->value.emplace_back(u64(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 3:
                    this->value.emplace_back(u128(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 4:
                    this->value.emplace_back(u256(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 5:
                    this->value.emplace_back(d64(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 6:
                    this->value.emplace_back(boolean(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 7:
                    this->value.emplace_back(collection(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 8:
                    this->value.emplace_back(str(boost::json::serialize(jv.as_array()[i].as_object()["value"])));
                    break;
                default:
                    break;
            }
        }
    }
};
#endif //VM_TEST_IMPLEMENTATION_COLLECTION_H