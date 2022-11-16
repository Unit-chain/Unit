//
// Created by Kirill Zhukov on 21.06.2022.
//

#ifndef UVM_JSONMERGEOPERATOR_H
#define UVM_JSONMERGEOPERATOR_H

#include "rocksdb/env.h"
#include "rocksdb/merge_operator.h"
#include "boost/algorithm/string.hpp"
#include "boost/json.hpp"
#include "boost/json/src.hpp"

class JsonMergeOperator : public  rocksdb::MergeOperator{
public:
    virtual bool FullMergeV2(const MergeOperationInput &merge_in, MergeOperationOutput *merge_out) const override;

    virtual bool PartialMerge(const rocksdb::Slice& key,
                              const rocksdb::Slice& left_operand,
                              const rocksdb::Slice& right_operand,
                              std::string* new_value,
                              rocksdb::Logger* logger) const override;
private:
    static std::vector<std::string> split(std::string str, char del);
};


std::vector<std::string> JsonMergeOperator::split(std::string str, char del){
    std::string temp = "";
    std::vector<std::string> words;
    for(char i : str){
        if(i != del){
            temp += i;
        }
        else{
            boost::trim(temp);
            words.push_back(temp);
            temp = "";
        }
    }
    words.push_back(temp);
    return words;
}

bool JsonMergeOperator::FullMergeV2(const MergeOperationInput &merge_in, MergeOperationOutput *merge_out) const {
    boost::json::value obj{};
    if (merge_in.existing_value) {
        try {
            obj = boost::json::parse(merge_in.existing_value->ToString());
        } catch (std::exception &e) {
            rocksdb::Log(merge_in.logger, "Invalid json string after parsing: %s", merge_in.existing_value->ToString().c_str());
            return false;
        }
    }

    for (const auto &value : merge_in.operand_list) {
        std::vector<std::string> split_vector = JsonMergeOperator::split(value.data_, '=');
        obj.as_object()[split_vector[0]] = split_vector[1];
    }

    merge_out->new_value =  boost::json::value_to<std::string>(obj);
    return true;
}

bool JsonMergeOperator::PartialMerge(const rocksdb::Slice &key, const rocksdb::Slice &left_operand,
                                     const rocksdb::Slice &right_operand, std::string *new_value,
                                     rocksdb::Logger *logger) const {
    auto split_vector1 = JsonMergeOperator::split(left_operand.ToString(), '=');   // "xyz[0] = 5" might return ["xyz[0]", 5] as an std::vector, etc.
    auto split_vector2 = JsonMergeOperator::split(right_operand.ToString(), '=');

    // If the two operations update the same value, just take the later one.
    if (split_vector1[0] == split_vector2[0]) {
        new_value->assign(right_operand.data(), right_operand.size());
        return true;
    } else {
        return false;
    }
}
#endif //UVM_JSONMERGEOPERATOR_H
