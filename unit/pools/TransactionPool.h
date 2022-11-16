//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_TRANSACTIONPOOL_H
#define UNIT_TRANSACTIONPOOL_H
#include "iostream"
#include "TransactionPoolErrors.h"
#include "../libdevcore/datastructures/containers/vector.h"
#include "../libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"

class TransactionPool {
public:
    TransactionPool() = default;
    void emplaceBack(const ValidTransaction& validTransaction);
    void dropTransaction(ValidTransaction &validTransaction);
    /// will be dropped transactions in: [vector::begin(); vector::begin()+tillPosition)
    void dropTransactions(long tillPosition);
    std::variant<ValidTransaction, std::exception> getTransaction(uint64_t index);
    const unit::vector<ValidTransaction> *getTxPool() const { return &this->transactionPool; }
    uint64_t getPoolSize();

    /// Errors checking
    /// should be used as function for checking if error equals to other
    template<class U, class V>
    inline static bool isSameErrors(U &u, V &v) { return std::is_same_v<decltype(u), decltype(v)>; }
    /// should be used as function for checking if error equals to other
    template<class U, class V>
    inline static bool isSameError(U &u) { return std::is_same_v<decltype(u), V>; }
    /// should be used as function for checking if error equals to other
    template<class U, class V>
    inline static bool isSameError(U &u, V &v) { return std::is_same_v<U, V>; }
private:
    mutable std::mutex mutex;
    unit::vector<ValidTransaction> transactionPool{};
};

void TransactionPool::emplaceBack(const ValidTransaction& validTransaction) {
    std::lock_guard<std::mutex> lock(mutex);
    this->transactionPool.emplace_back(validTransaction);
}

void TransactionPool::dropTransaction(ValidTransaction &validTransaction) {
    std::lock_guard<std::mutex> lock(mutex);
    this->transactionPool.remove(validTransaction);
}

std::variant<ValidTransaction, std::exception> TransactionPool::getTransaction(uint64_t index) {
    std::lock_guard<std::mutex> lock(mutex);
    if (index > this->transactionPool.size()) return unit::error::OutOfPoolRange();
    return this->transactionPool.at(index);
}

void TransactionPool::dropTransactions(long tillPosition) {
    std::lock_guard<std::mutex> lock(mutex);
    this->transactionPool.erase(this->transactionPool.begin(), this->transactionPool.begin()+tillPosition);
}

uint64_t TransactionPool::getPoolSize() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->transactionPool.size();
}

#endif //UNIT_TRANSACTIONPOOL_H
