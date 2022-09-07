//
// Created by Kirill Zhukov on 21.06.2022.
//

#ifndef UNIT_CHAIN_DB_H
#define UNIT_CHAIN_DB_H

#include "optional"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/env.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/compaction_filter.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"
#include "rocksdb/table.h"
#include "rocksdb/table_properties.h"
#include "cassert"
#include "vector"
#include "iterator"
#include "algorithm"
#include "../Block.h"
#include "../Wallet/WalletAccount.h"
#include "../Token/Token.h"
#include "../Hex.h"
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
static std::string kkDBPath = "/tmp/unit_db/";
const char DBPath[] = "/tmp/unit_db/";
const int cpuss = (int) std::thread::hardware_concurrency();
#endif

#define UNIT_TRANSFER 0
#define CREATE_TOKEN 1
#define TOKEN_TRANSFER 2

namespace unit {
    class DB {
    public:
        const std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor(ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions())};
        static bool push_block(Block block);
        static bool push_transactions(Block *block);
        static std::optional<std::string> get_balance(std::string &address);
        static std::optional<std::string> get_block_height();
        static std::optional<std::string> get_token(std::string &token_address);
        static std::optional<std::string> find_transaction(std::string tx_hash);
        static void close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles);
        static void close_iterators_DB(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::vector<rocksdb::Iterator*> *iterators);

    private:
        static std::vector<rocksdb::ColumnFamilyDescriptor> get_column_families();
        static rocksdb::Options get_db_options();
        static inline void normalize_str(std::string *str) {
            str->erase(std::remove(str->begin(), str->end(), '\"'),str->end());
        }
    };
}

#endif //UNIT_CHAIN_DB_H