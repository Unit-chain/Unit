//
// Created by Kirill Zhukov on 31.05.2022.
//

#ifndef UVM_VM_H
#define UVM_VM_H
#include <stack>
#include "vector"
#include "string"

class VM {
private:
    std::stack<void*> instructions;
public:
    VM();
    virtual ~VM();
    [[noreturn]]
    void run();
    void popInstruction();
    bool pushInstruction(void* instruction);
    bool isEmptyInstructions() {
        return this->instructions.empty();
    };
    std::vector<void*> memory;
    std::vector<void*> instructionsReferences;
};


#endif //UVM_VM_H
