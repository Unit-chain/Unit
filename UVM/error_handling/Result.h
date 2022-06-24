//
// Created by Kirill Zhukov on 08.06.2022.
//

#ifndef UVM_RESULT_H
#define UVM_RESULT_H
#include "string"

template <class T>
class Result {
public:
    Result(T value);

    Result(T value, const std::string &message);

    Result(T val, const std::string &message, const std::string &supportingResult);

    bool ok();
    T get_value();
    std::string get_message();

    [[nodiscard]] std::string &getSupportingResult();

    void setSupportingResult(const std::string &supportingResult);

protected:
    T val;
    std::string message;
    std::string supporting_result;
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

template<class T>
std::string &Result<T>::getSupportingResult() {
    return supporting_result;
}

template<class T>
void Result<T>::setSupportingResult(const std::string &supportingResult) {
    supporting_result = supportingResult;
}

template<class T>
Result<T>::Result(T val, const std::string &message, const std::string &supportingResult):val(val), message(message),
                                                                                          supporting_result(
                                                                                                  supportingResult) {
    this->val = val;
    this->message = message;
    this->supporting_result = supportingResult;
}


#endif //UVM_RESULT_H
