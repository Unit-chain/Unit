#include <assert.h>
#include <string.h>
#include "rocksdb/db.h"
#include <iostream>
#include <nlohmann/json.hpp>
int main(){
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());
    db->Put(rocksdb::WriteOptions(), "hi1", "heeeellloo");

    return 0;
}
