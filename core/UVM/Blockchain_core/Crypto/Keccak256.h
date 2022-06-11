//
// Created by Kirill Zhukov on 11.06.2022.
//

#ifndef UVM_KECCAK256_H
#define UVM_KECCAK256_H

#pragma once

#include <cstddef>
#include <cstdint>
#include "vector"
#include "iostream"
#include "algorithm"


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
    static inline uint64_t rotl64(uint64_t x, int i); // Requires 0 <= i <= 63
    static constexpr int BLOCK_SIZE = 200 - HASH_LEN * 2;
    static constexpr int NUM_ROUNDS = 24;
    static const unsigned char ROTATION[5][5]; //    Keccak256() = delete; Not instantiable
};

#endif //UVM_KECCAK256_H
