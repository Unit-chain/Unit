#include <iostream>
#include "vector"
#include "map"
#include "optional"
#include <boost/json/src.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_to.hpp>
#include "boost/json/value_from.hpp"
#include "boost/json/array.hpp"
#include "boost/unordered_map.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "string"
#include "UVM/DataTypes/Object.h"
#include "UVM/DataTypes/u128.h"
#include "UVM/DataTypes/boolean.h"
#include "UVM/DataTypes/i32.h"
#include "UVM/DataTypes/u64.h"
#include "UVM/DataTypes/u256.h"
#include "UVM/DataTypes/d64.h"
#include "UVM/DataTypes/str.h"
#include "UVM/DataTypes/collection.h"
#include "UVM/DataTypes/Pools/UniqueConstantPool.h"
#include "stack"
#include "array"
#include <algorithm>
#include <iterator>
#include "timeTest/LogDuration.h"
#include "UVM/VM.h"

void pop(std::vector<int> &vec) {
    vec.pop_back();
}

void print_vector(std::vector<Object> &vector) {
    for(int i = 0; i < vector.size(); i++)
        std::cout << "\"" << i << "\" = " << vector[i] << std::endl;
    std::cout << "---------------------" << std::endl;
}

void swap(Object *r, Object *s){
    Object temp = *r;
    *r = *s;
    *s = temp;
}

void swapInt(int *r, int *s){
    int temp = *r;
    *r = *s;
    *s = temp;
}

inline Object copy(Object &obj) {
    Object object;
    object.object = obj.object;
    object.type = obj.type;
    return object;
}

inline bool null_object(Object &object) {
    return object.isNull();
}

typedef std::vector<Object> Registers;

int main() {
    {
        LogDuration logDuration = LogDuration("test#1");
        std::string code = R"({"functions": [{"test": [27, 48, 53, 29, 62, 13, 112, 1, 1, 1, 3, 0]}], "values": [{"value":"test","type":0}]})";
        UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
        std::string function = "test";
        VM vm = VM(10, code);
        boost::json::array test = uniqueConstantPool.loadFunction(function).value();
        VM::VMRun(&vm, test);
        std::cout << vm.stack.back() << std::endl;
    }
    return 0;
}