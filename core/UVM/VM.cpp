//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "VM.h"
//#include "Opcodes.h"
#include "iostream"

VM::VM() {}
VM::~VM() {}

void VM::generate_block() {
    std::cout << "start to generate blocks";
    add: {
        if (this->block_lock) {
            goto add;
        } else {
            Blockchain_db blockchainDb = Blockchain_db();
            for (auto it : currentblock.transactions) {
                blockchainDb.push_transaction(it);
            }
            blockchainDb.push_block(currentblock);
            goto generate;
        }
    };

    generate: {
        currentblock = Block(1);
        sleep(1000*10); // 1000 millisecond * 10 = 10 seconds
        goto add;
    };
}

bool VM::push_transaction(std::string &transaction) {
    nlohmann::json transaction_json;
    try {
        transaction_json = nlohmann::json::parse(transaction);
    } catch (std::exception &e) {
        return false;
    }
    if  (!transaction_json.contains("extradata") || !transaction_json["extradata"].contains("name") || !transaction_json["extradata"].contains("value") || !transaction_json["extradata"].contains("bytecode")) return false;
    if  (transaction_json["extradata"]["name"].empty() || transaction_json["extradata"]["value"].empty() || transaction_json["extradata"]["bytecode"].empty()) return false;
    std::map<std::string, std::string> map = {{"name", to_string(transaction_json["extradata"]["name"])}, {"value", to_string(transaction_json["extradata"]["value"])}, {"bytecode", to_string(transaction_json["extradata"]["bytecode"])}};
    Transaction tx = Transaction(to_string(transaction_json["from"]), to_string(transaction_json["to"]), transaction_json["type"],  map, "0", transaction_json["amount"]);
    Blockchain_db blockchainDb = Blockchain_db();
    if (!blockchainDb.validate_sender_balance(tx)) return false;
    this->transactions_deque.push_back(tx);
    return true;
}

void foo() {
    std::cout << "test" << std::endl;
}

void VM::run() {
//    this->instructionsReferences.push_back(&&loop);
//    this->instructionsReferences.push_back(&&inc);

    std::thread thr(generate_block());
    thr.detach();
    std::cout << "test" << std::endl;
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

//    swap: {
//        auto ptrB = std::move(ptrA);
//    };
    inc: {
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
