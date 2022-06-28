#include "VM.h"
#include "Blockchain_core/Crypto/SHA3/sha3.h"


int main(){
    SHA3 sha3;
    std::cout << sha3("test") << std::endl;
    VM vm = VM();
    vm.run();
}
