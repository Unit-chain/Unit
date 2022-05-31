//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "VM.h"
#include "iostream"

VM::VM() {}
VM::~VM() {}

[[noreturn]]
void VM::run() {
    START: {
        this->instructionsReferences.push_back(&&LOOP);
        this->instructionsReferences.push_back(&&ADD);
        goto LOOP;
    }

    LOOP: {
        if (this->instructions.empty()) {
            goto LOOP;
        }
    }

    ADD: {
        goto LOOP;
    };

    CREATE_TX: {

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
