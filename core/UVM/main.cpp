#include <iostream>
#include "VM.h"
#include <cassert>

int main() {
    VM vm = VM();
    test: {};
    vm.pushInstruction(&&test);
    vm.run();
    return 0;
}
