////
//// Created by Kirill Zhukov on 21.06.2022.
////
//
//#include "JsonMergeOperator.h"
//
//std::vector<std::string> JsonMergeOperator::split(std::string str, char del){
//    std::string temp = "";
//    std::vector<std::string> words;
//    for(char i : str){
//        if(i != del){
//            temp += i;
//        }
//        else{
//            boost::trim(temp);
//            words.push_back(temp);
//            temp = "";
//        }
//    }
//    words.push_back(temp);
//    return words;
//}
//
//bool JsonMergeOperator::FullMerge(const rocksdb::Slice &key, const rocksdb::Slice *existing_value,
//                                  const std::deque<std::string> &operand_list, std::string *new_value,
//                                  rocksdb::Logger *logger) const {
//    nlohmann::json obj;
//    if (existing_value) {
//        try {
//            obj = nlohmann::json::parse(existing_value->ToString());
//        } catch (std::exception &e) {
//            rocksdb::Log(logger, "Invalid json string after parsing: %s", existing_value->ToString().c_str());
//            return false;
//        }
//    }
//
//    for (const auto &value : operand_list) {
//        std::vector<std::string> split_vector = JsonMergeOperator::split(value, '=');
//        obj[split_vector[0]] = split_vector[1];
//    }
//
//    *new_value = obj.dump();
//    return true;
//}
//
//bool JsonMergeOperator::PartialMerge(const rocksdb::Slice &key, const rocksdb::Slice &left_operand,
//                                     const rocksdb::Slice &right_operand, std::string *new_value,
//                                     rocksdb::Logger *logger) const {
//    auto split_vector1 = JsonMergeOperator::split(left_operand.ToString(), '=');   // "xyz[0] = 5" might return ["xyz[0]", 5] as an std::vector, etc.
//    auto split_vector2 = JsonMergeOperator::split(right_operand.ToString(), '=');
//
//    // If the two operations update the same value, just take the later one.
//    if (split_vector1[0] == split_vector2[0]) {
//        new_value->assign(right_operand.data(), right_operand.size());
//        return true;
//    } else {
//        return false;
//    }
//}
//
//
