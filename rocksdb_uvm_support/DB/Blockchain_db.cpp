//
// Created by Kirill Zhukov on 12.06.2022.
//

#include "Blockchain_db.h"

Result<bool> Blockchain_db::start_node_db() {
    std::cout << "test" << std::endl;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;
    options.create_missing_column_families = true;
    options.IncreaseParallelism(cpus);
    options.OptimizeLevelStyleCompaction();
    options.bottommost_compression = rocksdb::kLZ4Compression;
    options.compression = rocksdb::kLZ4Compression;
    options.max_background_jobs = cpus;
    options.delete_obsolete_files_period_micros = 300000000;
    options.keep_log_file_num = 5;
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
    return (status.ok()) ? Result<bool>(true) : Result<bool>(status.ok(), "column families already exists");
}



