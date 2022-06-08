//
// Created by Kirill Zhukov on 08.06.2022.
//

#ifndef UVM_RESULT_H
#define UVM_RESULT_H

template <class T>
class result {
public:
    explicit result(T value);
    bool isPresent();
    T get_value();

protected:
    T val;
};

template<class T>
result<T>::result(T value) {
    this->val = value;
}

template<class T>
bool result<T>::isPresent() {
    return val != nullptr;
}

template<class T>
T result<T>::get_value() {
    return this->val;
}


#endif //UVM_RESULT_H
