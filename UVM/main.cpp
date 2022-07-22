#include "VM.h"
//#include "rocksdb/db.h"
//#include "rocksdb/options.h"
//#include "rocksdb/slice.h"
//#include "rocksdb/utilities/transaction.h"
//#include "rocksdb/utilities/optimistic_transaction_db.h"
//#include "rocksdb/snapshot.h"
//#include "iostream"
//
//#include <unistd.h>
//#include <thread>
//static std::string kkDBPath = "/tmp/unit_db/";
////const char DBPath[] = "/tmp/unit_db/";
//const int cpuss = (int) std::thread::hardware_concurrency();
//
//rocksdb::Options get_db_options() {
//    rocksdb::Options options;
//    options.create_if_missing = false;
//    options.error_if_exists = false;
//    options.IncreaseParallelism(cpuss);
//    options.OptimizeLevelStyleCompaction();
//    options.bottommost_compression = rocksdb::kZSTD;
//    options.compression = rocksdb::kLZ4Compression;
//    options.max_background_jobs = cpuss;
//    options.delete_obsolete_files_period_micros = 300000000;
//    options.env->SetBackgroundThreads(cpuss);
//    options.keep_log_file_num = 5;
//    return options;
//}
//
//std::vector<rocksdb::ColumnFamilyDescriptor> get_column_families() {
//    const std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies = {rocksdb::ColumnFamilyDescriptor("blockTX", rocksdb::ColumnFamilyOptions()),
//                                                                         rocksdb::ColumnFamilyDescriptor("addressContracts", rocksdb::ColumnFamilyOptions()),
//                                                                         rocksdb::ColumnFamilyDescriptor("tx", rocksdb::ColumnFamilyOptions()),
//                                                                         rocksdb::ColumnFamilyDescriptor("height", rocksdb::ColumnFamilyOptions()),
//                                                                         rocksdb::ColumnFamilyDescriptor("accountBalance", rocksdb::ColumnFamilyOptions()),
//                                                                         rocksdb::ColumnFamilyDescriptor(ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions())};
//    return *&columnFamilies;
//}

int main(){
//    rocksdb::DB *db;
//    std::vector<rocksdb::ColumnFamilyHandle*> handles;
//    rocksdb::Status status = rocksdb::DB::Open(get_db_options(), kkDBPath, get_column_families(), &handles, &db);
//
//    status = db->Put(rocksdb::WriteOptions(), handles[4], rocksdb::Slice("test"), rocksdb::Slice("zhopa"));
//    std::string find;
//    status = db->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice("test"), &find);
//
//    std::cout << find;

    VM vm = VM();
    vm.run();
}



//rocksdb::Options options;
//std::vector<rocksdb::ColumnFamilyHandle*> handles;
//rocksdb::OptimisticTransactionDBOptions optimisticTransactionDbOptions;
//rocksdb::OptimisticTransactionDB* txn_db;
//
//rocksdb::Status s = rocksdb::OptimisticTransactionDB::Open(rocksdb::DBOptions(), optimisticTransactionDbOptions, kkDBPath, get_column_families(), &handles, &txn_db);
//assert(s.ok());
////    db = txn_db->GetBaseDB();
//rocksdb::WriteOptions write_options;
//rocksdb::ReadOptions read_options;
//rocksdb::OptimisticTransactionOptions txn_options;
//std::string value;
//
//// Start a transaction
//rocksdb::Transaction* txn = txn_db->BeginTransaction(write_options);
//assert(txn);
//
//// Read a key in this transaction
////    s = txn->Get(read_options, "abc", &value);
//std::string to_find;
//s = txn->Get(rocksdb::ReadOptions(), handles[4], rocksdb::Slice("zhopa"), &to_find);
////    assert(s.IsNotFound());
//std::cout << to_find << std::endl;
//
//// Write a key in this transaction
////    s = txn->Put("abc", "xyz");
//s = txn->PutUntracked(handles[4], rocksdb::Slice("zhopa"), rocksdb::Slice("test"));
//assert(s.ok());
//
//// Commit transaction
//s = txn->Commit();