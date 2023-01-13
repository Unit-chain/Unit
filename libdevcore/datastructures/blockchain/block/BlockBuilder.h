//
// Created by Kirill Zhukov on 02.11.2022.
//

#ifndef UNIT_BLOCKBUILDER_H
#define UNIT_BLOCKBUILDER_H
#include <utility>

#include "iostream"
#include "Block.h"
#include "bip44/BIP44.hpp"
#include "bip44/ecdsa.hpp"

class BlockBuilder {
public:
    BlockBuilder() = default;
    explicit BlockBuilder(BIP44Result &bip44Result) : bip44Result(bip44Result) {}

    inline BlockBuilder *setBlock(Block *block) {
        this->currentBlock = block;
        return this;
    }

    inline BlockBuilder *setPreviousHash(const std::string &previousHash) {
        this->blockHeader.setPreviousHash(previousHash);
        return this;
    }

    inline BlockBuilder *setTime(uint64_t time) {
        this->blockHeader.setTime(time);
        return this;
    }

    inline BlockBuilder *setVersion(uint64_t version) {
        this->blockHeader.setVersion(version);
        return this;
    }

    inline BlockBuilder *setIndex(uint64_t index) {
        this->blockHeader.setIndex(index);
        return this;
    }

    inline BlockBuilder *setNonce(int nonce) {
        this->blockHeader.setNonce(nonce);
        return this;
    }

    inline BlockBuilder *setBlockHeader() {
        this->currentBlock->setBlockHeader(this->blockHeader);
        return this;
    }

    inline BlockBuilder *setRewardProverAddress(const std::string &proverAddress) {
        this->currentBlock->setRewardProverAddress(proverAddress);
        return this;
    }

    inline BlockBuilder *setMessage(const std::string &message) {
        this->currentBlock->setMessage(message);
        return this;
    }

    inline BlockBuilder *setEpoch(std::string epoch) {
        this->currentBlock->setEpoch(epoch);
        return this;
    }

    inline BlockBuilder *generateHash() {
        this->currentBlock->generateHash();
        return this;
    }

    inline BlockBuilder *generateRoot() {
        this->currentBlock->generateMerkleRoot();
        return this;
    }

    inline BlockBuilder *setProverSign() {
        ECDSASignResult sig = ecdsa_sign_message(this->currentBlock->serializeForSigning(), this->bip44Result.prv);
        this->currentBlock->setRp(sig.r);
        this->currentBlock->setSp(sig.s);
        this->currentBlock->setSignP(sig.message_hash);
        return this;
    }

    inline BlockBuilder *insertShard(std::optional<Shard> &shard) {
        if (!shard.has_value()) return this;
        this->currentBlock->shardList.emplace_back(*shard);
        return this;
    }

    inline Block *build() {
        return this->currentBlock;
    }

    [[nodiscard]] Block getCurrentBlock() const {
        return *currentBlock;
    }
    void setCurrentBlock(Block *currentBlock) {
        this->currentBlock = currentBlock;
    }
    const BlockHeader &getBlockHeader() const {
        return blockHeader;
    }
    void setBlockHeader(const BlockHeader &blockHeader) {
        BlockBuilder::blockHeader = blockHeader;
    }
private:
    Block *currentBlock;
    BlockHeader blockHeader;
    BIP44Result bip44Result;
};
#endif //UNIT_BLOCKBUILDER_H
