#include <iostream>
#include "sstream"
#include "libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"
#include "libdevcore/datastructures/request/RawTransaction.h"
#include "libdevcore/utils/StringUtil.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    #if 1
        std::string str = R"({
          "instruction": "i_push_transaction",
          "data": {
            "from": "g2px1",
            "to": "merchant",
            "amount": 1.000003,
            "type": 0,
            "extradata": {
              "name": "a",
              "value": 1,
              "bytecode":"a"
            },
            "sign":"sadwa",
            "r":"sadwa",
            "s":"sadwa",
            "fee":0.0001
          }
        })";

        RawTransaction *rawTransaction = RawTransaction::parse(&str);
        rawTransaction->generateHash();
        std::cout << *rawTransaction->serializeToJsonTransaction() << std::endl;
    #endif
    #if 0
        std::string test = "Hello, World!";
        std::cout << *StringUtil::encode64(&test) << std::endl;
        double i1 = 222.11; int i2 = 22; int i3 = 33;
        std::string test2 = "[%s]: %s {i1=%f, i2=%d, i3=%d}";
        std::cout << *StringUtil::insertSubElement(&test2, "DEBUG", "Xyz failed", i1, i2, i3) << std::endl;
        std::string test3 = "[%s]: %s, %d";
        std::cout << *StringUtil::insertSubElement(&test3, "DEBUG", "Xyz failed", 1)  << std::endl;
    #endif
    return 0;
}
