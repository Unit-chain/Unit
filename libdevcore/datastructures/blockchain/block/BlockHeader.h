//
// Created by Kirill Zhukov on 23.10.2022.
//

#ifndef UNIT_BLOCKHEADER_H
#define UNIT_BLOCKHEADER_H
#include "iostream"
#include "sstream"
#include "crypto/SHA3/sha3.h"

struct BlockHeader {
    BlockHeader() = default;
    BlockHeader(std::string hash, std::string previousHash, std::string merkleRoot,
                uint64_t time, uint64_t version, uint64_t transactionCount, uint64_t index, uint64_t size,
                int nonce) : hash(std::move(hash)), previousHash(std::move(previousHash)), merkleRoot(std::move(merkleRoot)), time(time),
                             version(version), transactionCount(transactionCount), index(index), size(size),
                             nonce(nonce) {}
    BlockHeader(const std::string &previousHash, uint64_t time, uint64_t version, uint64_t index, int nonce);

    std::string hash;
    std::string previousHash;
    std::string merkleRoot;
    uint64_t time{};
    uint64_t version{};
    uint64_t transactionCount{};
    uint64_t index{};
    uint64_t size = 0;
    int nonce{};

    std::string generateHash();
    std::string serializeBlockHeader() const;
    uint64_t getSize();

    const std::string &getHash() const;

    void setHash(const std::string &hash);

    const std::string &getPreviousHash() const;

    void setPreviousHash(const std::string &previousHash);

    const std::string &getMerkleRoot() const;

    void setMerkleRoot(const std::string &merkleRoot);

    uint64_t getTime() const;

    void setTime(uint64_t time);

    uint64_t getVersion() const;

    void setVersion(uint64_t version);

    uint64_t getTransactionCount() const;

    void setTransactionCount(uint64_t transactionCount);

    uint64_t getIndex() const;

    void setIndex(uint64_t index);

    void setSize(uint64_t size);

    int getNonce() const;

    void setNonce(int nonce);

    inline virtual BlockHeader &operator=(const BlockHeader &c2) {
        this->hash = c2.hash;
        this->previousHash = c2.previousHash;
        this->merkleRoot = c2.merkleRoot;
        this->time = c2.time;
        this->version = c2.time;
        this->version = c2.version;
        this->transactionCount = c2.transactionCount;
        this->index = c2.index;
        this->size = c2.size;
        this->nonce = c2.nonce;
        return *this;
    }
};

std::string BlockHeader::serializeBlockHeader() const {
    std::stringstream ss;
    ss << R"({"hash":")" << this->hash << R"(", "previousHash":")" << this->previousHash << R"(", "transactionMerkleRoot":")" << this->merkleRoot << R"(", "time":)"
    << this->time << R"(, "version":)" << this->version << R"(, "transactionCount":)" << this->transactionCount << R"(, "index":)"
    << this->index << R"(, "size":)" << this->size << R"(, "nonce":)" << this->nonce;
    return ss.str();
}

BlockHeader::BlockHeader(const std::string &previousHash, uint64_t time, uint64_t version, uint64_t index, int nonce)
        : previousHash(previousHash), time(time), version(version), index(index), nonce(nonce) {}

const std::string &BlockHeader::getHash() const {
    return hash;
}

void BlockHeader::setHash(const std::string &hash) {
    BlockHeader::hash = hash;
}

const std::string &BlockHeader::getPreviousHash() const {
    return previousHash;
}

void BlockHeader::setPreviousHash(const std::string &previousHash) {
    BlockHeader::previousHash = previousHash;
}

const std::string &BlockHeader::getMerkleRoot() const {
    return merkleRoot;
}

void BlockHeader::setMerkleRoot(const std::string &merkleRoot) {
    BlockHeader::merkleRoot = merkleRoot;
}

uint64_t BlockHeader::getTime() const {
    return time;
}

void BlockHeader::setTime(uint64_t time) {
    BlockHeader::time = time;
}

uint64_t BlockHeader::getVersion() const {
    return version;
}

void BlockHeader::setVersion(uint64_t version) {
    BlockHeader::version = version;
}

uint64_t BlockHeader::getTransactionCount() const {
    return transactionCount;
}

void BlockHeader::setTransactionCount(uint64_t transactionCount) {
    BlockHeader::transactionCount = transactionCount;
}

uint64_t BlockHeader::getIndex() const {
    return index;
}

void BlockHeader::setIndex(uint64_t index) {
    BlockHeader::index = index;
}

void BlockHeader::setSize(uint64_t size) {
    BlockHeader::size = size;
}

int BlockHeader::getNonce() const {
    return nonce;
}

void BlockHeader::setNonce(int nonce) {
    BlockHeader::nonce = nonce;
}

#endif //UNIT_BLOCKHEADER_H
