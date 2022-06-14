//
// Created by Kirill Zhukov on 31.05.2022.
//

#ifndef UVM_VM_H
#define UVM_VM_H
#include "deque"
#include "vector"
#include "thread"
#include "string"
#include "nlohmann/json.hpp"
#include "Blockchain_core/Transaction.h"
#include "Blockchain_core/DB/Blockchain_db.h"

class VM {
public:
    VM();
    virtual ~VM();
    void run();
    void popInstruction();
    bool pushInstruction(void* instruction);
    bool push_transaction(std::string &transaction);

private:
    std::vector<void*> memory;
    std::vector<void*> instructionsReferences;
    std::deque<Transaction> transactions_deque;
    std::stack<void*> instructions;
    Block currentblock;
    bool block_lock = false;

    [[noreturn]] void generate_block();
};


#endif //UVM_VM_H
