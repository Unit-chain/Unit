////
//// Created by Kirill Zhukov on 10.06.2022.
////
//
//#include <string>
//#include <map>
//#include "Transaction.h"
//
//Transaction::Transaction() {}
//
//Transaction::~Transaction() {}
//
//const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &
//Transaction::getFrom() const {
//    return from;
//}
//
//void
//Transaction::setFrom(const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &from) {
//    Transaction::from = from;
//}
//
//const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &Transaction::getTo() const {
//    return to;
//}
//
//void
//Transaction::setTo(const std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> &to) {
//    Transaction::to = to;
//}
//
//uint64_t Transaction::getType() const {
//    return type;
//}
//
//void Transaction::setType(uint64_t type) {
//    Transaction::type = type;
//}
//
//uint64_t Transaction::getDate() const {
//    return date;
//}
//
//void Transaction::setDate(uint64_t date) {
//    Transaction::date = date;
//}
//
//const std::__1::map<std::string, std::string> &Transaction::getExtraData() const {
//    return extra_data;
//}
//
//void Transaction::setExtraData(const std::__1::map<std::string, std::string> &extraData) {
//    extra_data = extraData;
//}
