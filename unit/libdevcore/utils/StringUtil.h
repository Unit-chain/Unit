//
// Created by Kirill Zhukov on 21.10.2022.
//

#ifndef UNIT_STRINGUTIL_H
#define UNIT_STRINGUTIL_H

#include "cinttypes"
#include "iostream"
#include "optional"
#include "boost/archive/iterators/binary_from_base64.hpp"
#include "boost/archive/iterators/base64_from_binary.hpp"
#include "boost/archive/iterators/transform_width.hpp"
#include "boost/algorithm/string.hpp"

class StringUtil {
public:
    static std::shared_ptr<std::string> insertSubElement(const std::string *sFormat...);
    static std::shared_ptr<std::string> decode64(const std::string *val);
    static std::shared_ptr<std::string> encode64(const std::string *val);
};


#endif //UNIT_STRINGUTIL_H
