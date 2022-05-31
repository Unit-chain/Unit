//
// Created by Kirill Zhukov on 31.05.2022.
//

#include "VM.h"
#include "iostream"

VM::VM() {}
VM::~VM() {}

[[noreturn]]
void VM::run() {
    LOOP: {
        if (this->isEmptyInstructions()) {
            goto LOOP;
        } else {
            goto ADD;
        }
    }

    ADD: {

        goto LOOP;
    };
}

void VM::popInstruction() {
    this->instructions.pop();
}

bool VM::pushInstruction(void *instruction) {
    try {
        this->instructions.push(instruction);
    } catch (const std::exception& e){
        return false;
    }
}
