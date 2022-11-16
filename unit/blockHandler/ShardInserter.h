//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_SHARDINSERTER_H
#define UNIT_SHARDINSERTER_H
#include "iostream"
#include "../libdevcore/datastructures/blockchain/block/Shard.h"
#include "../libdevcore/datastructures/blockchain/block/Block.h"

class InvalidBlockAccess : public std::exception {
    std::string what() {
        return "invalid access to block";
    }
};

class ShardInserter {
public:
    [[maybe_unused]] virtual std::optional<std::exception> insertShard(Shard &shard) = 0;
};

class BasicLocalShardInserter : public ShardInserter{
public:
    BasicLocalShardInserter(bool *lock, Block *currentBlock) : lock(lock), currentBlock(currentBlock) {}
    std::optional<std::exception> insertShard(Shard &shard) override;
private:
    bool *lock;
    Block *currentBlock;
};

std::optional<std::exception> BasicLocalShardInserter::insertShard(Shard &shard) {
    if (this->lock) return InvalidBlockAccess();
    currentBlock->emplaceBack(shard);
    return std::nullopt;
}

#endif //UNIT_SHARDINSERTER_H
