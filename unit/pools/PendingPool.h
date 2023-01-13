//
// Created by Kirill Zhukov on 22.11.2022.
//

#ifndef UNIT_PENDINGPOOL_H
#define UNIT_PENDINGPOOL_H

#include "iostream"
#include "TransactionPoolErrors.h"
#include "boost/unordered_map.hpp"
#include "datastructures/blockchain/transaction/Transaction.h"

class PendingPool {
public:
    PendingPool() = default;
    void emplaceBack(Transaction& validTransaction);
    template<class T>
    /// will be dropped transaction by hash
    void dropTransaction(T &&hash);

    template<class T>
    Transaction getTransaction(T &&hash);
    template<class T>
    bool hasTransaction(T &&hash);
    uint64_t getPoolSize();
private:
    mutable std::mutex mutex;
    boost::unordered_map<std::string, Transaction> pool{};
};

template<class T>
Transaction PendingPool::getTransaction(T &&hash) {
    if (!std::is_same_v<decltype(hash), std::string>) throw unit::error::InvalidHashType();
    return this->pool.at(hash);
}

template<class T>
bool PendingPool::hasTransaction(T &&hash) {
    if (!std::is_same_v<decltype(hash), std::string>) throw unit::error::InvalidHashType();
    return this->pool.contains(hash);
}

void PendingPool::emplaceBack(Transaction &validTransaction) {
    if (this->pool.contains(validTransaction.hash)) return;
    validTransaction.generateHash();
    this->pool.emplace(validTransaction.hash, validTransaction);
}

template<class T>
void PendingPool::dropTransaction(T &&hash) {
    if (!std::is_same_v<decltype(hash), std::string>) throw unit::error::InvalidHashType();
    this->pool.erase(hash);
}

uint64_t PendingPool::getPoolSize() {
    return this->pool.size();
}


#endif //UNIT_PENDINGPOOL_H
