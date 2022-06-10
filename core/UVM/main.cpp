#include <assert.h>
#include <string.h>
#include "rocksdb/db.h"
#include <iostream>
#include <nlohmann/json.hpp>

#include <thread>
#include "vector"

using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
#define kDBPath "/tmp/testdb"
int main(){
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
    std::string key2 = "test_key";
    std::string value = "test_value";
    std::string fetched_value;
    if (status.ok()) status = db->Put(rocksdb::WriteOptions(), key2, value);
    delete db;
    return 0;
}
