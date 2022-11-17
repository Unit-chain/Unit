//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_TRANSACTIONPOOLERRORS_H
#define UNIT_TRANSACTIONPOOLERRORS_H
#include "exception"
#include "string"

namespace unit::error {
    class OutOfPoolRange : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override {
            return "went out of transaction pool size";
        }
    };
}

#endif //UNIT_TRANSACTIONPOOLERRORS_H
