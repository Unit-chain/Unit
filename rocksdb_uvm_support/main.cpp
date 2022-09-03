#include <iostream>
#include "DB/DB.h"

int main() {
    std::cout << "start creating DB path..." << std::endl;
    DB blockchainDb = DB();
    std::cout << "test" << std::endl;
    Result<bool> result = blockchainDb.start_node_db();
    return 0;
}
