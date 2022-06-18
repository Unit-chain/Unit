//
// Created by Kirill Zhukov on 13.06.2022.
//

#ifndef UVM_OPTIONAL_H
#define UVM_OPTIONAL_H
#include "iostream"
template <class T, class V>
class Optional {
public:
    Optional(T value);

    Optional(T value, V flag, const std::string &message);

    Optional(T val, const std::string &message, const std::string &supportingOptional);

    bool ok();
    T get_value();
    std::string get_message();

    const std::string &getSupportingOptional() const;

    void setSupportingOptional(const std::string &supportingOptional);

protected:
    T val;
    V errors;
    std::string message;
    std::string supporting_Optional;
};

template<class T, class V>
Optional<T, V>::Optional(T value) {
    this->val = value;
}

template<class T, class V>
bool Optional<T, V>::ok() {
    return errors;
}


template<class T, class V>
T Optional<T, V>::get_value() {
    return this->val;
}

template<class T, class V>
Optional<T, V>::Optional(T value, V flag, const std::string& message) {
    this->val = value;
    this->message = message;
    this->errors = flag;
}

template<class T, class V>
std::string Optional<T, V>::get_message() {
    return this->message;
}

template<class T, class V>
const std::string &Optional<T, V>::getSupportingOptional() const {
    return supporting_Optional;
}

template<class T, class V>
void Optional<T, V>::setSupportingOptional(const std::string &supportingOptional) {
    supporting_Optional = supportingOptional;
}

template<class T, class V>
Optional<T, V>::Optional(T val, const std::string &message, const std::string &supportingOptional):val(val), message(message),
                                                                                          supporting_Optional(
                                                                                                  supportingOptional) {
    this->val = val;
    this->message = message;
    this->supporting_Optional = supportingOptional;
}


#endif //UVM_OPTIONAL_H
