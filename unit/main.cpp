#include <iostream>
#include "libdevcore/datastructures/request/TransactionRequest.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string str = R"(
                {
          "instruction": "i_push_transaction",
          "data": {
            "from": "g2px1",
            "to": "merchant",
            "amount": 1000.00003,
            "type": 0,
            "extradata": {
              "name": "null",
              "value": "null",
              "bytecode":"null"
            }
          }
        }
    )";
//    std::optional<TransactionRequest> transactionRequest = TransactionRequest::parse(&str);
//    std::cout << transactionRequest.has_value() << std::endl;
    return 0;
}
