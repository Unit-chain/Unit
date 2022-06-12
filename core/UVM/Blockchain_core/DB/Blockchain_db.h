//
// Created by Kirill Zhukov on 12.06.2022.
//

#ifndef UVM_BLOCKCHAIN_DB_H
#define UVM_BLOCKCHAIN_DB_H
#include "../../ENV/env.h"
#include "rocksdb/db.h"
#include "cassert"
#include "Unit_CF.h"
#include "vector"

template<class T>
class Blockchain_db {
public:
    void start_node_db();
    /* column families for blockchain database
     * blockTX - stores data about blocks
     * addressContracts maps contract address to total number of transactions, number of non contract transactions
     * tx - stores transactions
     * height - maps block height to block hash and additional data about block
     * accountBalance - stores balances of each user's address
     */
    std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
                                                                   rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
                                                                   rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
                                                                   rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
                                                                   rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions())};

};

#endif //UVM_BLOCKCHAIN_DB_H
