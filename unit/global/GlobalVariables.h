//
// Created by Kirill Zhukov on 20.10.2022.
//

#ifndef UNIT_GLOBALVARIABLES_H
#define UNIT_GLOBALVARIABLES_H
#include "../libdevcore/crypto/SHA3/sha3.h"
#define logger std::cout
#define DEBUG true

namespace hash {
    [[maybe_unused]] SHA3 sha3_224 = SHA3(SHA3::Bits224);
    [[maybe_unused]] SHA3 sha3_256 = SHA3(SHA3::Bits256);
    [[maybe_unused]] SHA3 sha3_384 = SHA3(SHA3::Bits384);
    [[maybe_unused]] SHA3 sha3_512 = SHA3(SHA3::Bits512);
}

#endif //UNIT_GLOBALVARIABLES_H
