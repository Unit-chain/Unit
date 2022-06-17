//
// Created by Kirill Zhukov on 12.06.2022.
//

#ifndef UVM_BLOCKCHAIN_DB_H
#define UVM_BLOCKCHAIN_DB_H
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "cassert"
#include "vector"
#include "iterator"
#include "algorithm"
#include "../../error_handling/Result.h"
#include "../Block.h"
#include "../Wallet/WalletAccount.h"
#include "../Token/Token.h"
/// utility structures
#if defined(OS_WIN)
#include <Windows.h>
    static std::string kDBPath = "C:\\Windows\\TEMP\\unit";
    const char DBPath[] = "C:\\Windows\\TEMP\\unit";
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    const int cpus = (int) system_info.dwNumberOfProcessors;
#else
#include <unistd.h>
#include <thread>
    static std::string kDBPath = "/tmp/unit_db/";
    const char DBPath[] = "/tmp/unit_db/";
const int cpus = (int) std::thread::hardware_concurrency();
#endif

#define UNIT_TRANSFER 0
#define CREATE_TOKEN 1
#define TOKEN_TRANSFER 2

class Blockchain_db {
public:
    Result<bool> start_node_db();
    /* column families for blockchain database
     * blockTX - stores data about blocks [0]
     * addressContracts maps contract address to total number of transactions, number of non contract transactions [1]
     * tx - stores transactions [2]
     * height - maps block height to block hash and additional data about block [3]
     * accountBalance - stores balances of each user's address [4]
     * ROCKSDB_NAMESPACE::kDefaultColumnFamilyName - default CF [5]
     */
    const std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions()),
                                                                         rocksdb::ColumnFamilyDescriptor(ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions())
    };
    bool validate_sender_balance(Transaction &transaction);
    const std::vector<std::string> columnFamiliesNames = {"blockTX", "addressContracts", "tx", "height", "accountBalance"};
    Result<bool> push_block(Block &block);
    Result<bool> push_transaction(Transaction &transaction);
    Result<bool> create_new_token(std::string &name, double supply, std::string &creator, std::string bytecode, Transaction &transaction);
    Result<bool> get_balance(std::string &address);
    Result<bool> get_block_height();
};

#endif //UVM_BLOCKCHAIN_DB_H