//
// Created by Kirill Zhukov on 12.06.2022.
//

#ifndef UVM_UNIT_CF_H
#define UVM_UNIT_CF_H
#include "string"

enum UNIT_CF {
    blockTX,
    addressContracts,
    tx,
    height,
    accountBalance
};

UNIT_CF& operator ++ (UNIT_CF& e) {
    e = UNIT_CF(static_cast<std::underlying_type<UNIT_CF>::type>(e) + 1);
    return e;
}

constexpr const char* UNIT_CF_to_string(UNIT_CF e) throw() {
    switch (e) {
        case UNIT_CF::blockTX: return "blockTX";
        case UNIT_CF::addressContracts: return "addressContracts";
        case UNIT_CF::tx: return "tx";
        case UNIT_CF::height: return "height";
        case UNIT_CF::accountBalance: return "accountBalance";
        default: throw std::invalid_argument("Unimplemented item");
    }
}
#endif //UVM_UNIT_CF_H
