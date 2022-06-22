//
// Created by Kirill Zhukov on 22.06.2022.
//

#ifndef UVM_HMAC_512_H
#define UVM_HMAC_512_H
#include "iostream"
#include "../SHA512/SHA512.h"
#include "../../Hex.h"

namespace unit {
    class HMAC_512 {
    public:
        static std::string get_hmac(std::string key, const std::string &msg, bool is_hex = true);

    private:
        static std::string to_hex(const std::string& input);
        static std::string get_hash(const std::string &input);
    };
}

#endif //UVM_HMAC_512_H
