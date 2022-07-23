//
// Created by Kirill Zhukov on 11.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_UNIQUECONSTANTPOOL_H
#define VM_TEST_IMPLEMENTATION_UNIQUECONSTANTPOOL_H
#include "iostream"
#include "boost/unordered_map.hpp"
#include "boost/json.hpp"
#include "boost/json/object.hpp"
#include "boost/json/array.hpp"
#include "boost/json/value_from.hpp"
#include "boost/json/value_to.hpp"
#include <boost/json/src.hpp>
#include "optional"
#include "vector"
#include "../Object.h"
#include "../u128.h"
#include "../boolean.h"
#include "../i32.h"
#include "../u64.h"
#include "../u256.h"
#include "../d64.h"
#include "../str.h"
#include "../collection.h"

class UniqueConstantPool {
public:
    UniqueConstantPool() = default;

    inline explicit UniqueConstantPool(const std::string& str) {
        boost::json::value parsed = boost::json::parse(str);
        this->programPool = parsed.at("functions");
        this->deserialize(parsed.at("values"));
    }

    inline std::optional<boost::json::array> loadFunction(std::string &name) {
        std::tuple exist = this->function_exist(name);
        if (!std::get<0>(exist))
            return std::nullopt;
        return this->programPool.as_array()[std::get<1>(exist)].at(name).as_array();
    }

    inline Object loadReference(int index) {
        return dataValues[index];
    }

    inline void updateReferenceValue(int index, Object &value) {
        this->dataValues[index] = value;
    }

    inline std::string serialize(){
        boost::json::object obj;
        obj.emplace("functions", programPool);
        obj.emplace("values", boost::json::parse(this->serializeValues()));
        return boost::json::serialize(obj);
    }

private:
    boost::json::value programPool;
    std::vector<Object> dataValues;

    inline std::string serializeValues() {
        std::stringstream stringstream;
        stringstream << "[";
        for(int i = 0; i < dataValues.size(); i++) {
            switch (dataValues[i].type) {
                case 1:
                    stringstream << (i32) dataValues[i];
                    break;
                case 2:
                    stringstream << (u64) dataValues[i];
                    break;
                case 3:
                    stringstream << (u128) dataValues[i];
                    break;
                case 4:
                    stringstream << (u256) dataValues[i];
                    break;
                case 5:
                    stringstream << (d64) dataValues[i];
                    break;
                case 6:
                    stringstream << (boolean) dataValues[i];
                    break;
                case 7:
                    stringstream << (collection) dataValues[i];
                    break;
                case 8:
                    stringstream << (str) dataValues[i];
                    break;
                default:
                    stringstream << (Object) dataValues[i];
                    break;
            }
            (i != dataValues.size() - 1) ? stringstream << ',' : stringstream << ']';
        }
        return stringstream.str();
    }

    inline void deserialize(boost::json::value jv) {
        for (int i = 0; i < jv.as_array().size(); i++) {
            switch (jv.as_array()[i].as_object()["type"].as_int64()) {
                case 0:
                    this->dataValues.emplace_back(Object(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 1:
                    this->dataValues.emplace_back(i32(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 2:
                    this->dataValues.emplace_back(u64(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 3:
                    this->dataValues.emplace_back(u128(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 4:
                    this->dataValues.emplace_back(u256(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 5:
                    this->dataValues.emplace_back(d64(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 6:
                    this->dataValues.emplace_back(boolean(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 7:
                    this->dataValues.emplace_back(collection(boost::json::serialize(jv.as_array()[i].at("value"))));
                    break;
                case 8:
                    this->dataValues.emplace_back(str(boost::json::serialize(jv.as_array()[i].as_object()["value"])));
                    break;
                default:
                    break;
            }
        }
    }

    std::tuple<bool, int> function_exist(std::string &name) {
        for(int i = 0; i < this->programPool.as_array().size(); i++)
            if (this->programPool.as_array()[i].as_object().contains(name))
                return std::make_tuple(true, i);
        return std::make_tuple(true, -1);
    }
};


//    boost::json::value parser = boost::json::parse(str);
//    std::vector<std::vector<int>> vector1(parser.as_array().size());
//    for(int i = 0; i < parser.as_array().size(); i++) {
//        std::cout << *boost::json::value_to<std::vector<int>>(parser.as_array()[i]).begin() << std::endl;
//        vector1[i] = {boost::json::value_to<std::vector<int>>(parser.as_array()[i]).begin(), boost::json::value_to<std::vector<int>>(parser.as_array()[i]).end()};
//    }

#endif //VM_TEST_IMPLEMENTATION_UNIQUECONSTANTPOOL_H




////
//// Created by Kirill Zhukov on 02.07.2022.
////
//
//#ifndef VM_TEST_IMPLEMENTATION_VM_H
//#define VM_TEST_IMPLEMENTATION_VM_H
//#include "stack"
//#include "iostream"
//#include <cstdlib>
//#include "DataTypes/Object.h"
//#include "DataTypes/u256.h"
//#include "DataTypes/u128.h"
//#include "DataTypes/u64.h"
//#include "DataTypes/i32.h"
//#include "DataTypes/d64.h"
//#include "DataTypes/boolean.h"
//#include "DataTypes/str.h"
//#include "DataTypes/collection.h"
//#include "DataTypes/Pools/UniqueConstantPool.h"
//#include "boost/json/array.hpp"
//#include "exception"
//
//typedef std::vector<Object> Registers;
//
//namespace vm {
//    inline void swap(Object *r, Object *s){
//        Object temp = *r;
//        *r = *s;
//        *s = temp;
//    }
//
//    inline void remove_from_locals(Registers &reg, Object &obj){
//        reg.erase(std::remove(reg.begin(), reg.end(), obj), reg.end());
//    }
//
//    inline Object copy(Object &obj) {
//        Object object;
//        object.object = obj.object;
//        object.type = obj.type;
//        return object;
//    }
//
//    inline bool stringContains(std::string &str, char c) {
//        return ((str.find(c) != std::string::npos));
//    }
//
//    void print_registers(Registers &vector) {
//        for(int i = 0; i < vector.size(); i++)
//            std::cout << "\"" << i << "\" = " << vector[i] << std::endl;
//        std::cout << "---------------------" << std::endl;
//    }
//}
//
//class VM {
//public:
//    inline VM() = default;
//
//    inline explicit VM(uint16_t contractSize, std::string &code) : contract_size(contractSize), uniqueConstantPool(code) {
//        if (contractSize > 25536) throw std::overflow_error("size of contract should be lower then 25536 bytes");
//    }
//
//    inline static void VMRun(VM *vm, boost::json::array &stack_val) {
//        boost::json::array::iterator it = stack_val.begin();
//        void *dtt[] = {
//                &&stop,
//                &&go_to,
//                &&swap_references,
//                &&iadd,
//                &&isub,
//                &&idivide,
//                &&imul,
//                &&imod,
//                &&ixor,
//                &&iinv,
//                &&ilshift,
//                &&irshift,
//                &&pop,
//                &&_,
//                &&dup,
//                &&ior,
//                &&iand,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&_,
//                &&iconst_0,
//                &&iconst_1,
//                &&iconst_2,
//                &&iconst_3,
//                &&iconst_4,
//                &&u64const_0,
//                &&u64const_1,
//                &&u64const_2,
//                &&u64const_3,
//                &&u64const_4,
//                &&u128const_0,
//                &&u128const_1,
//                &&u128const_2,
//                &&u128const_3,
//                &&u128const_4,
//                &&u256const_0,
//                &&u256const_1,
//                &&u256const_2,
//                &&u256const_3,
//                &&u256const_4,
//                &&ostore_0,
//                &&ostore_1,
//                &&ostore_2,
//                &&ostore_3,
//                &&ldc,
//                &&oload_0,
//                &&oload_1,
//                &&oload_2,
//                &&oload_3,
//                &&swap,
//                &&if_acmpeq,
//                &&if_acmpne,
//                &&if_icmpeq,
//                &&if_icmpge,
//                &&if_icmpgt,
//                &&if_icmple,
//                &&if_icmplt,
//                &&if_icmpne,
//                &&ifeq,
//                &&ifge,
//                &&ifgt,
//                &&ifle,
//                &&iflt,
//                &&ifne,
//                &&ifnonnull,
//                &&ifnull,
//                &&nop,
//                &&collectionlength,
//                &&dconst_0,
//                &&dconst_1,
//                &&u64add,
//                &&u128add,
//                &&u256add,
//                &&dadd,
//                &&u64sub,
//                &&u128sub,
//                &&u256sub,
//                &&dsub,
//                &&u64divide,
//                &&u64mul,
//                &&u64mod,
//                &&u64xor,
//                &&u64inv,
//                &&u64lshift,
//                &&u128divide,
//                &&u128mul,
//                &&u128mod,
//                &&u128xor,
//                &&u128inv,
//                &&u256divide,
//                &&u256mul,
//                &&u256mod,
//                &&u256xor,
//                &&u256inv,
//                &&u256lshift,
//                &&dinv,
//                &&ddivide,
//                &&dmul,
//                &&u64or,
//                &&u64and,
//                &&u128or,
//                &&u128and,
//                &&u256or,
//                &&u256and,
//                &&dor,
//                &&iinc,
//                &&i2d,
//                &&i2u64,
//                &&i2u128,
//                &&i2u256,
//                &&d2i,
//                &&d2u64,
//                &&d2u128,
//                &&d2u256,
//                &&u64_2u128,
//                &&u64_2u256,
//                &&u128_2u256,
//                &&u128lshift,
//                &&u64rshift,
//                &&u128rshift,
//                &&u256rshift,
//                &&iconst_5,
//                &&u64const_5,
//                &&u128const_5,
//                &&u256const_5,
//                &&u64inc,
//                &&u128inc,
//                &&u256inc
//        }; // Direct Threading Table
//        goto *dtt[(it)->as_int64()];
//
//        _:{goto *dtt[(++it)->as_int64()];}; // placeholder
//
//        stop:{
//            return;
//        };
//
//        ldc: {
//            vm->stack.emplace_back(vm->uniqueConstantPool.loadReference(boost::json::value_to<int>(*(++it))));
//            vm->ip+=2;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ostore_0: {
//            if(std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->locals[0] = vm->stack.back();
//            vm->stack.pop_back();
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ostore_1: {
//            vm::print_registers(vm->locals);
//            if(std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->locals[1] = vm->stack.back();
//            vm->stack.pop_back();
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ostore_2: {
//            if(std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->locals[2] = vm->stack.back();
//            vm->stack.pop_back();
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ostore_3: {
//            if(std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->locals[3] = vm->stack.back();
//            vm->stack.pop_back();
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        oload_0:{
//            if (std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->stack.emplace_back(vm->locals[0]);
//            vm::remove_from_locals(vm->locals, vm->locals[0]);
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        oload_1:{
//            if (std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            Registers::iterator reg_it = vm->locals.begin();
//            vm->stack.emplace_back(vm->locals[1]);
////            *(reg_it += 1) = Object();
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        oload_2:{
//            if (std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->stack.emplace_back(vm->locals[2]);
//            vm::remove_from_locals(vm->locals, vm->locals[2]);
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        oload_3:{
//            if (std::count_if(vm->locals.begin(), vm->locals.end(), null_object) == 0)
//                throw std::logic_error("Registers are full");
//            vm->stack.emplace_back(vm->locals[3]);
//            vm::remove_from_locals(vm->locals, vm->locals[3]);
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        swap:{
//            vm::swap(&(vm->stack.back()), &(vm->stack[vm->stack.size()-2]));
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        swap_references:{
//            vm::swap(&(vm->stack[vm->stack.size() - (++it)->as_int64()]), &(vm->stack[vm->stack.size() - (++it)->as_int64()]));
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        go_to:{
//            vm->ip++;
//            uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//            it = (stack_val.begin() + a);
//            vm->ip = a;
//            goto *dtt[(it)->as_int64()];
//        };
//
//        iadd:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2) + i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64add:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2) + u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128add:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2) + u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256add:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2) + u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dadd:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = d64((d64)*(vm->stack.end()-=2) + d64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        isub:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2) - i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64sub:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2) - u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128sub:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2) - u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256sub:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2) - u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dsub:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = d64((d64)*(vm->stack.end()-=2) - d64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        idivide:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2) / i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64divide:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2) / u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128divide:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2) / u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256divide:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2) / u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ddivide:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = d64((d64)*(vm->stack.end()-=2) / d64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        imul:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2) * i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64mul:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2) * u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128mul:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2) * u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256mul:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2) * u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto
//            *dtt[(++it)->as_int64()];
//        };
//
//        dmul:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = d64((d64)*(vm->stack.end()-=2) * d64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        // bug is under
//
//        imod: {
//            vm->ip++;
//            (i32)(*(vm->stack.end()-=2)) %= (i32) (vm->stack.back());
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2) % i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64mod:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2) % u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128mod:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2) % u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256mod:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2) % u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ixor:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2)  ^ i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64xor:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2)  ^ u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128xor:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2)  ^ u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256xor:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2)  ^ u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iinv:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = (i32)*(vm->stack.end()-=2);
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64inv: {
//            vm->ip++;
//            !(u64)(*(vm->stack.end()-=2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128inv:{
//            vm->ip++;
//            !(u128)(*(vm->stack.end()-=2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256inv:{
//            vm->ip++;
//            (u256)(*(vm->stack.end()-=2)) *= (u256) (vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dinv:{
//            vm->ip++;
//            (d64)(*(vm->stack.end()-=2)) *= (d64) (vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ilshift:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32((i32)*(vm->stack.end()-=2)  << i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64lshift:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64((u64)*(vm->stack.end()-=2)  << i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128lshift:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128((u128)*(vm->stack.end()-=2)  << i32(vm->stack.back()).value);
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256lshift:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256((u256)*(vm->stack.end()-=2)  << i32(vm->stack.back()).value);
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        irshift:{
//            vm->ip++;
//            (*(vm->stack.end() -= 2)) = i32((i32) *(vm->stack.end() -= 2) >> i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64rshift: {
//            vm->ip++;
//            (*(vm->stack.end() -= 2)) = u64((u64) *(vm->stack.end() -= 2) >> i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128rshift: {
//            vm->ip++;
//            (*(vm->stack.end() -= 2)) = u128((u128) *(vm->stack.end() -= 2) >> i32(vm->stack.back()).value);
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256rshift: {
//            vm->ip++;
//            (*(vm->stack.end() -= 2)) = u256((u256) *(vm->stack.end() -= 2) >> i32(vm->stack.back()).value);
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        pop:{
//            vm->ip++;
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dup:{
//            vm->ip++;
//            vm->stack.emplace_back(vm::copy(vm->stack.back()));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        ior:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32(i32(*(vm->stack.end()-=2)) ||  i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64or:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64(u64(*(vm->stack.end()-=2)) ||  u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128or:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128(u128(*(vm->stack.end()-=2)) ||  u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256or:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256(u256(*(vm->stack.end()-=2)) ||  u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dor:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = d64(d64(*(vm->stack.end()-=2)) ||  d64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iand:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = i32(i32(*(vm->stack.end()-=2)) &&  i32(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64and:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u64(u64(*(vm->stack.end()-=2)) &&  u64(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128and:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u128(u128(*(vm->stack.end()-=2)) && u128(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256and:{
//            vm->ip++;
//            (*(vm->stack.end()-=2)) = u256(u256(*(vm->stack.end()-=2)) &&  u256(vm->stack.back()));
//            vm->stack.pop_back();
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        // bug is above
//
//        iconst_0:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iconst_1:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(1));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iconst_2:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iconst_3:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(3));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iconst_4:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(4));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iconst_5:{
//            vm->ip++;
//            vm->stack.emplace_back(i32(5));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_0:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_1:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(1));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_2:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_3:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(3));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_4:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(4));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64const_5:{
//            vm->ip++;
//            vm->stack.emplace_back(u64(5));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128const_0: {
//            vm->ip++;
//            vm->stack.emplace_back(u128(0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128const_1: {
//            vm->ip++;
//            vm->stack.emplace_back(u128(1));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128const_2: {
//            vm->ip++;
//            vm->stack.emplace_back(u64(2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128const_3: {
//            vm->ip++;
//            vm->stack.emplace_back(u128(3));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128const_4:{
//            vm->ip++;
//            vm->stack.emplace_back(u128(4));
//            goto
//            *dtt[(++it)->as_int64()];
//        };
//
//        u128const_5:{
//            vm->ip++;
//            vm->stack.emplace_back(u128(5));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256const_0:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256const_1:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(1));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256const_2:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(2));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256const_3:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(3));
//            goto
//            *dtt[(++it)->as_int64()];
//        };
//
//        u256const_4:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(4));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256const_5:{
//            vm->ip++;
//            vm->stack.emplace_back(u256(5));
//            goto*dtt[(++it)->as_int64()];
//        };
//
//        if_acmpeq:{
//            vm->ip+=2;
//            if(vm->stack.back() == (*(vm->stack.end()-=2))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_acmpne:{
//            vm->ip+=2;
//            if (vm->stack.back() != (*(vm->stack.end() -= 2))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmpeq:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) == u256(*(*(vm->stack.end() -= 2)).object) : i32(*vm->stack.back().object) == i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmpge:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) >= u256(*(*(vm->stack.end() -= 2)).object) : i32(*vm->stack.back().object) >= i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmpgt:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) > u256(*(*(vm->stack.end() -= 2)).object) : i32(*vm->stack.back().object) > i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmple:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) <= u256(*(*(vm->stack.end() -= 2)).object)
//                                             : i32(*vm->stack.back().object) <= i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmplt:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) < u256(*(*(vm->stack.end() -= 2)).object)
//                                             : i32(*vm->stack.back().object) <
//                                               i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        if_icmpne:{
//            vm->ip+=2;
//            if ((vm->stack.back().type != 1) ? u256(*vm->stack.back().object) != u256(*(*(vm->stack.end() -= 2)).object)
//                                             : i32(*vm->stack.back().object) !=
//                                               i32(*((*(vm->stack.end() -= 2)).object))) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifeq:{
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) == i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifge: {
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) >= i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifgt:{
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) > i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifle:{
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) <= i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        iflt:{
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) < i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifne:{
//            vm->ip+=2;
//            if (i32(*vm->stack.back().object) != i32(0)) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifnonnull:{
//            vm->ip+=2;
//            if (!vm->stack.back().isNull()) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        ifnull: {
//            vm->ip+=2;
//            if (vm->stack.back().isNull()) {
//                uint16_t a = boost::json::value_to<uint16_t>(*(++it)) - 1;
//                it = (stack_val.begin() + a);
//                vm->ip = a;
//            }
//
//            goto *dtt[(it += 2)->as_int64()];
//        };
//
//        nop: {
//            vm->ip++;
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        collectionlength:{
//            vm->ip++;
//            if(vm->stack.back().type == 7)
//                vm->stack.emplace_back(i32(((collection) vm->stack.back()).size()));
//            else
//                throw std::logic_error("not a collection");
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dconst_0:{
//            vm->ip++;
//            vm->stack.emplace_back(d64(0.0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        dconst_1:{
//            vm->ip++;
//            vm->stack.emplace_back(d64(1.0));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        iinc:{
//            vm->ip+=3;
////            (*(vm->locals.begin() += (++it)->as_int64())) = (i32)vm->locals[it->as_int64()] + i32(boost::json::value_to<int>(*(++it)));
////            std::cout << "it: " << (++it)->as_int64() << std::endl;
//            int index = boost::json::value_to<int>(*(++it));
//            (*(vm->locals.begin() += index)) = i32((i32)vm->locals[it->as_int64()] + i32(boost::json::value_to<int>(*(++it))));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        i2d:{
//            vm->ip++;
//            *(vm->stack.end()) = d64(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        i2u64:{
//            vm->ip++;
//            *(vm->stack.end()) = u64(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        i2u128:{
//            vm->ip++;
//            *(vm->stack.end()) = u128(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        i2u256:{
//            vm->ip++;
//            *(vm->stack.end()) = u256(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        d2i:{
//            vm->ip++;
//            *(vm->stack.end()) = d64(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        d2u64:{
//            vm->ip++;
//            *(vm->stack.end()) = u64(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        d2u128:{
//            vm->ip++;
//            *(vm->stack.end()) = u128(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        d2u256:{
//            vm->ip++;
//            *(vm->stack.end()) = u256(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64_2u128:{
//            vm->ip++;
//            *(vm->stack.end()) = u128(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64_2u256:{
//            vm->ip++;
//            *(vm->stack.end()) = u256(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128_2u256:{
//            vm->ip++;
//            *(vm->stack.end()) = u256(vm->stack.back());
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u64inc:{
//            vm->ip++;
//            *(vm->locals.begin() += (++it)->as_int64()) = u64(u64(*(vm->locals.begin() += (it)->as_int64())) + u64(boost::json::value_to<uint64_t>(*(++it))));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u128inc:{
//            vm->ip++;
//            *(vm->locals.begin() += (++it)->as_int64()) = u128(u64(*(vm->locals.begin() += (it)->as_int64())) + u128(boost::json::value_to<std::string>(*(++it))));
//            goto *dtt[(++it)->as_int64()];
//        };
//
//        u256inc:{
//            vm->ip++;
//            *(vm->locals.begin() += (++it)->as_int64()) = u256(u64(*(vm->locals.begin() += (it)->as_int64())) + u256(boost::json::value_to<std::string>(*(++it))));
//            goto *dtt[(++it)->as_int64()];
//        };
//    }
//
//    inline static Object invokeAdditionalFunction(VM *vm){
//        return Object();
//    }
//
//    template<class T>
//    inline T returnObjectWithType(Object &obj){
//        switch (obj.type) {
//            case 1:
//                return (i32) obj;
//                break;
//            case 2:
//                return (u64) obj;
//                break;
//            case 3:
//                return (u128) obj;
//                break;
//            case 4:
//                return (u256) obj;
//                break;
//            case 5:
//                return (d64) obj;
//                break;
//            case 6:
//                return (boolean) obj;
//                break;
//            case 7:
//                return (collection) obj;
//                break;
//            case 8:
//                return (str) obj;
//                break;
//            default:
//                return (Object) obj;
//                break;
//        }
//    }
//
//    static bool null_object(Object &obj) {
//        return obj.isNull();
//    }
//
//    uint16_t contract_size;
//    UniqueConstantPool uniqueConstantPool;
//    std::vector<Object> stack;
//private:
//    Registers locals{Object(), Object(), Object(), Object()}; // local storage
//    uint32_t ip = 0;
//};
//
//
//#endif //VM_TEST_IMPLEMENTATION_VM_H
