//
// Created by Kirill Zhukov on 22.10.2022.
//

#ifndef UNIT_LAZY_POINTER_H
#define UNIT_LAZY_POINTER_H
#define DEBUG true
#include "iostream"
#include <thread>
#include <mutex>

template<class T>
class lazy_pointer {
public:
    explicit lazy_pointer(T value);
    lazy_pointer(lazy_pointer<T> &a) noexcept;
    virtual ~lazy_pointer();

    T *getValue() const;
    bool destroy();
    void setStatus(short status);
    [[nodiscard]] int getCnt();

    short getStatus() const;

private:
    T *value;
    int *cnt{};
    /// if status == 0 || status == 1 - it won't be killed now.
    /// if status == 3 - it won't be killed at anytime program are running.
    ///  if status == -1 - means value is in garbage and can be deleted at any time.
    short status = 0;
};

template<class T>
lazy_pointer<T>::lazy_pointer(T value) : value(&value)  {
    this->cnt = new int(1);
}

template<class T>
lazy_pointer<T>::lazy_pointer(lazy_pointer<T> &a) noexcept : value(a.value), cnt(a.cnt){
    ++(*(this->cnt));
}

template<class T>
lazy_pointer<T>::~lazy_pointer() {
    --(*(this->cnt));
    #if DEBUG
        std::cout << "cnt in thread: #" << std::this_thread::get_id() << std::endl;
    #endif
}

template<class T>
T *lazy_pointer<T>::getValue() const {
    return value;
}

template<class T>
int lazy_pointer<T>::getCnt() {
    return *cnt;
}

template<class T>
short lazy_pointer<T>::getStatus() const {
    return status;
}

template<class T>
bool lazy_pointer<T>::destroy() {
    if (*(this->cnt) > 0) return false;
    delete this->cnt;
    delete this->value;
    delete this->cnt_mutex;
    delete this->status_mutex;
    return true;
}

template<class T>
void lazy_pointer<T>::setStatus(short status) {
    lazy_pointer::status = status;
}

#endif //UNIT_LAZY_POINTER_H
