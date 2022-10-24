//
// Created by Kirill Zhukov on 23.10.2022.
//

#ifndef UNIT_BLOCKHEADER_H
#define UNIT_BLOCKHEADER_H
#include "iostream"
#include "sstream"
#include "../../../crypto/SHA3/sha3.h"

struct BlockHeader {
    BlockHeader() = default;
    BlockHeader(std::string hash, std::string previousHash, std::string merkleRoot,
                uint64_t time, uint64_t version, uint64_t transactionCount, uint64_t index, uint32_t size,
                int nonce) : hash(std::move(hash)), previousHash(std::move(previousHash)), merkleRoot(std::move(merkleRoot)), time(time),
                             version(version), transactionCount(transactionCount), index(index), size(size),
                             nonce(nonce) {}

    std::string hash;
    std::string previousHash;
    std::string merkleRoot;
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t version{};
    uint64_t transactionCount{};
    uint64_t index{};
    uint32_t size = 0;
    int nonce{};

    std::string generateHash();
    std::string serializeBlockHeader();

    uint64_t getSize();
};

std::string BlockHeader::serializeBlockHeader() {
    std::stringstream ss;
    ss << R"({"hash":")" << this->hash << R"(", "previousHash":")" << this->previousHash << R"(", "merkleRoot":")" << this->merkleRoot << R"(", "time":)"
    << this->time << R"(, "version":)" << this->version << R"(, "transactionCount":)" << this->transactionCount << R"(, "index":)"
    << this->index << R"(", "size":)" << this->size << R"(, "nonce":)" << this->nonce << R"(})";
    return std::string();
}

#endif //UNIT_BLOCKHEADER_H
