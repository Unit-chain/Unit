#include "rocksdb/db.h"
#include <iostream>
#include "VM.h"
#include "Server/Server.h"

int main(){
    VM vm = VM();
    vm.run();
//    Server::start_server();
    return 0;
}
