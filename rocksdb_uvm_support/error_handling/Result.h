//
// Created by Kirill Zhukov on 08.06.2022.
//

#ifndef UVM_RESULT_H
#define UVM_RESULT_H
#include "string"

template <class T>
class Result {
public:
    explicit Result(T value);
    explicit Result(T value, const std::string& message);
    bool ok();
    T get_value();
    std::string get_message();

protected:
    T val;
    std::string message;
};

template<class T>
Result<T>::Result(T value) {
    this->val = value;
}

template<class T>
bool Result<T>::ok() {
    return !val;
}

template<class T>
T Result<T>::get_value() {
    return this->val;
}

template<class T>
Result<T>::Result(T value, const std::string& message) {
    this->val = value;
    this->message = message;
}

template<class T>
std::string Result<T>::get_message() {
    return this->message;
}


#endif //UVM_RESULT_H
