//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "DB.h"

Result<bool> DB::start_node_db() {
    std::cout << "test" << std::endl;
    rocksdb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.IncreaseParallelism(cpuss);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kNoCompression;
    options.compression = rocksdb::kLZ4Compression;
    options.create_if_missing = true;
    options.create_missing_column_families = true;
    options.max_background_jobs = cpuss;
    options.env->SetBackgroundThreads(cpuss);
    options.num_levels = 2;
    options.merge_operator = nullptr;
    options.compaction_filter = nullptr;
    options.compaction_filter_factory = nullptr;
    options.rate_limiter = nullptr;
    options.max_open_files = -1;
    options.max_write_buffer_number = 6;
    options.max_background_flushes = cpuss;
    options.level0_stop_writes_trigger = -1;
    options.level0_slowdown_writes_trigger = -1;
    options.max_open_files = -1;

    rocksdb::Options balance_options;
    balance_options.create_if_missing = false;
    balance_options.error_if_exists = false;
    balance_options.IncreaseParallelism(cpuss);
    balance_options.OptimizeLevelStyleCompaction();
    balance_options.bottommost_compression = rocksdb::kNoCompression;
    balance_options.compression = rocksdb::kNoCompression;
    balance_options.create_if_missing = true;
    balance_options.create_missing_column_families = true;
    balance_options.max_background_jobs = cpuss;
    balance_options.env->SetBackgroundThreads(cpuss);
    balance_options.num_levels = 2;
    balance_options.merge_operator = nullptr;
    balance_options.compaction_filter = nullptr;
    balance_options.compaction_filter_factory = nullptr;
    balance_options.rate_limiter = nullptr;
    balance_options.max_open_files = -1;
    balance_options.max_write_buffer_number = 6;
    balance_options.max_background_flushes = cpuss;
    balance_options.level0_stop_writes_trigger = -1;
    balance_options.level0_slowdown_writes_trigger = -1;
    balance_options.max_open_files = 5000;
    balance_options.create_if_missing = true;
    balance_options.create_missing_column_families = true;

    rocksdb::DB* db;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);

    for (const auto & columnFamiliesName : columnFamiliesNames) { // creating column families
        rocksdb::ColumnFamilyHandle* cf;
        std::cout << "Creating column families: " << columnFamiliesName << std::endl;
        status = db->CreateColumnFamily(rocksdb::ColumnFamilyOptions(), columnFamiliesName, &cf);
        db->DestroyColumnFamilyHandle(cf); // delete ptr
    }
    delete db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    status = rocksdb::DB::Open(rocksdb::DBOptions(), kDBPath, this->columnFamilies, &handles, &db);
    for (auto handle : handles) {
        status = db->DestroyColumnFamilyHandle(handle);
        std::cout << "Destructing column families handler status: " << status.ok() << std::endl;
    }
    delete db;
    status = rocksdb::DB::Open(balance_options ,kkDBPath, &db);

    return (status.ok()) ? Result<bool>(true) : Result<bool>(status.ok(), "column families already exists");
}



