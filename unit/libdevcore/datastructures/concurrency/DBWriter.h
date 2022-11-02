//
// Created by Kirill Zhukov on 29.10.2022.
//

#ifndef UNIT_DBWRITER_H
#define UNIT_DBWRITER_H
#include <utility>
#include "iostream"
#include "deque"
#include "thread"
#include "../../db/DBProvider.h"

[[noreturn]] void commit(std::deque<WriteBatch*> *deque, dbProvider::BatchProvider* dbProvider) {
    while (true) {
        if (!deque->empty()) {
            operationDBStatus::DBCode dbCode = dbProvider->commitBatch(std::shared_ptr<WriteBatch>(deque->back()));
            std::cout << "in-thread committing code: " << (((uint16_t) dbCode == 0) ? 0 : (uint16_t) dbCode)  << std::endl;
        }
    }
}

class DBWriter {
public:
    explicit DBWriter(string blockPath) : dbProvider(std::move(blockPath)) {}
    static inline std::shared_ptr<rocksdb::WriteBatch> getBatch() { return dbProvider::BatchProvider::getBatch(); }
    inline dbProvider::BatchProvider *getProvider() { return &(this->dbProvider); };
    void init();

    [[nodiscard]] std::deque<WriteBatch *> *getDeque();
protected:
    std::deque<WriteBatch*> deque;
    dbProvider::BatchProvider dbProvider;
    std::thread dbThread;
};

void DBWriter::init() {
    dbThread = std::thread(commit, &(this->deque), &(this->dbProvider));
    dbThread.detach();
}

std::deque<WriteBatch *> *DBWriter::getDeque() {
    return &(this->deque);
}
#endif //UNIT_DBWRITER_H

