////
//// Created by Kirill Zhukov on 21.06.2022.
////
//
//#ifndef UVM_JSONMERGEOPERATOR_H
//#define UVM_JSONMERGEOPERATOR_H
//
//#include "boost/algorithm/string.hpp"
//#include <rocksdb/env.h>
//#include "rocksdb/merge_operator.h"
//#include "nlohmann/json.hpp"
//
//class JsonMergeOperator : public  rocksdb::MergeOperator{
//public:
//    virtual bool FullMerge(const rocksdb::Slice& key,
//                           const rocksdb::Slice* existing_value,
//                           const std::deque<std::string>& operand_list,
//                           std::string* new_value,
//                           rocksdb::Logger* logger) const override;
//
//    virtual bool PartialMerge(const rocksdb::Slice& key,
//                              const rocksdb::Slice& left_operand,
//                              const rocksdb::Slice& right_operand,
//                              std::string* new_value,
//                              rocksdb::Logger* logger) const override;
//private:
//    static std::vector<std::string> split(std::string str, char del);
//};
//
//
//#endif //UVM_JSONMERGEOPERATOR_H
