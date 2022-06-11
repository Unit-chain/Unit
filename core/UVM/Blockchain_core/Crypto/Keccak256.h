//
// Created by Kirill Zhukov on 11.06.2022.
//

#ifndef UVM_KECCAK256_H
#define UVM_KECCAK256_H

#pragma once

//#include <cstddef>
//#include <cstdint>
#include "vector"
#include "iostream"


/*
 * Computes the Keccak-256 hash of a sequence of bytes. The hash value is 32 bytes long.
 * Provides just one static method.
 */
class Keccak256 final {

public:
    static constexpr int HASH_LEN = 32;
    static std::string  getHash(std::string& str, int len);
private:
    static void absorb(std::uint64_t state[5][5]);
    static std::vector<std::byte> getBytes(std::string const &s);
    static constexpr int BLOCK_SIZE = 200 - HASH_LEN * 2;
    static constexpr int NUM_ROUNDS = 24;
    // Requires 0 <= i <= 63
    static std::uint64_t rotl64(std::uint64_t x, int i);
//    Keccak256() = delete;  // Not instantiable
    static const unsigned char ROTATION[5][5];
};

#endif //UVM_KECCAK256_H
