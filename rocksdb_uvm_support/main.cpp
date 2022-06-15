#include <iostream>
#include "DB/Blockchain_db.h"

int main() {
    std::cout << "start creating DB path..." << std::endl;
    Blockchain_db blockchainDb = Blockchain_db();
    std::cout << "test" << std::endl;
    Result<bool> result = blockchainDb.start_node_db();
    return 0;
}
