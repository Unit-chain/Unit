#include "BlockHandler.h"
#include "Blockchain_core/DB/DB.h"

int main(){
    BlockHandler vm = BlockHandler();
    vm.run();
//    rocksdb::DB *db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    unit::DB::getReadOnlyDb(&db, &handles);
//    std::string name;
//    db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice("g2px1"), &name);
//    std::cout << name << std::endl;
//    std::optional<std::string> op = unit::DB::getBalanceWithDb(&db, &handles);
//    if (op->empty())
//        std::cout << "error occurred" << std::endl;
//    else
//        std::cout << op.value() << std::endl;
}