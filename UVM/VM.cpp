//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "VM.h"
//#include "Opcodes.h"
#include "iostream"

VM::VM() {}
VM::~VM() {}

[[noreturn]] void VM::generate_block(Block *current, bool *lock) {
    std::cout << "Starting 'block generator'" << std::endl;
    add: {
        if (*lock) {
            goto add;
        } else {
            Blockchain_db blockchainDb = Blockchain_db();
            Result<bool> result = blockchainDb.get_block_height();
            nlohmann::json block_json = nlohmann::json::parse(result.getSupportingResult());
            uint64_t index = block_json["index"];
            for (Transaction &it : current->transactions) {
                it.setBlockId(index);
                Result<bool> res = blockchainDb.push_transaction(it);
                if(res.get_value()) {
                    it.setHash(res.getSupportingResult());
                } else {
                    current->transactions.erase(std::remove(current->transactions.begin(), current->transactions.end(), it), current->transactions.end());
                }
            }
            blockchainDb.push_block(*current, result);
            goto generate;
        }
    };

    generate: {
        *current = Block(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // 1000 millisecond * 10 = 10 seconds
        goto add;
    };
}

[[noreturn]] void VM::run() {
    std::thread th(VM::generate_block, &currentblock, &block_lock);
    th.detach();
    std::thread server_th(Server::start_server, &transactions_deque);
    loop: { // later need to check an instructions stack
        if (this->transactions_deque.empty()) {
            goto loop;
        } else {
            goto create_tx;
        }
    };

    create_tx: {
        this->block_lock = true;
        Transaction transaction = this->transactions_deque.front();
        currentblock.push_tx(transaction);
        this->transactions_deque.pop_front();
        this->block_lock = false;
        goto loop;
    };
}

void VM::popInstruction() {
    this->instructions.pop();
}

bool VM::pushInstruction(void *instruction) {
    try {
        this->instructions.push(instruction);
        return true;
    } catch (const std::exception& e){
        return false;
    }
}