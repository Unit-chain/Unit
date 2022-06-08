#include <assert.h>
#include <string.h>
#include "rocksdb/db.h"
#include <iostream>

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_simple_example";
#else
std::string kDBPath = "/tmp/testdb";
#endif

int main(){
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, kDBPath, &db);
    assert(status.ok());
    return 0;
}
