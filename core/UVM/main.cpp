#include <iostream>
#include "VM.h"

int main() {
    VM vm = VM();
    test: {};
    vm.pushInstruction(&&test);
    vm.run();
    return 0;
}
