//
// Created by Kirill Zhukov on 21.06.2022.
//

#ifndef UNIT_CHAIN_DB_H
#define UNIT_CHAIN_DB_H

#include "optional"
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
static std::string kDBPath = "/tmp/unit_db/";
const char DBPath[] = "/tmp/unit_db/";
const int cpus = (int) std::thread::hardware_concurrency();
#endif


namespace unit {
    class DB {
    public:
        const std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions()),
                                                                             rocksdb::ColumnFamilyDescriptor(ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions())};
        //.has_value()
        void push_block();
        std::optional<bool> validate_sender_balance(Transaction &transaction);
        void push_transaction(Transaction transaction);
        static std::optional<std::string> get_balance(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, std::string &address);
        static std::optional<std::string> get_block_height(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles);
        static void create_wallet(std::string &address, rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles);
        std::vector<rocksdb::ColumnFamilyHandle*> open_database(rocksdb::DB* db);
        std::vector<rocksdb::ColumnFamilyHandle*> open_read_only_database(rocksdb::DB* db);
        static void close_db(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles);

    private:
        static rocksdb::Options get_db_options();
        static std::optional<std::string> create_new_token(rocksdb::DB* db, std::vector<rocksdb::ColumnFamilyHandle*> *handles, Transaction *transaction);
        static inline void normalize_str(std::string *str) {
            str->erase(std::remove(str->begin(), str->end(), '\"'),str->end());
        }
    };
}

#endif //UNIT_CHAIN_DB_H
