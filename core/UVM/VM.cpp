//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "VM.h"
#include "Opcodes.h"
#include "iostream"

VM::VM() {}
VM::~VM() {}

[[noreturn]]
void VM::run() {
    this->instructionsReferences.push_back(&&loop);
    this->instructionsReferences.push_back(&&inc);
    goto loop;


    loop: {
        if (this->instructions.empty()) {
            goto loop;
        }
    }

    inc: {
        goto loop;
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
