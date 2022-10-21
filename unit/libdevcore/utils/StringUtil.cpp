//
// Created by Kirill Zhukov on 21.10.2022.
//

#include "StringUtil.h"

std::shared_ptr<std::string> StringUtil::decode64(const std::string *val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return std::make_shared<std::string>(boost::algorithm::trim_right_copy_if(std::string(It(std::begin(*val)), It(std::end(*val))), [](char c) {
        return c == '\0';
    }));
}

std::shared_ptr<std::string> StringUtil::encode64(const std::string *val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(*val)), It(std::end(*val)));
    return std::make_shared<std::string>(tmp.append((3 - val->size() % 3) % 3, '='));
}

std::shared_ptr<std::string> StringUtil::insertSubElement(const std::string *sFormat...) {
    const char *const zcFormat = sFormat->c_str();
    va_list vaArgs;
    va_start(vaArgs, sFormat);
    va_list vaCopy;
    va_copy(vaCopy, vaArgs);
    const int iLen = std::vsnprintf(nullptr, 0, zcFormat, vaCopy);
    va_end(vaCopy);
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    std::string response(zc.data(), zc.size());
    response.pop_back();
    return std::make_shared<std::string>(response);
}