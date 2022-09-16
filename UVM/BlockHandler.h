//
// Created by Kirill Zhukov on 31.05.2022.
//

#ifndef UVM_BLOCKHANDLER_H
#define UVM_BLOCKHANDLER_H
#include "deque"
#include "vector"
#include "thread"
#include "string"
#include "functional"
#include "stack"
#include "iterator"
#include "boost/json.hpp"
#include "Blockchain_core/Transaction.h"
#include "Blockchain_core/DB/DB.h"
#include "Server/Server.h"
#include "containers/list.h"

class BlockHandler {
public:
    BlockHandler();
    virtual ~BlockHandler();

    [[noreturn]] void run();

private:
    unit::list<Transaction> transactions_deque;
    Block currentblock = Block(1);
    bool block_lock = false;

    [[noreturn]] static void generate_block(Block *current, bool *lock);
};


#endif //UVM_BLOCKHANDLER_H