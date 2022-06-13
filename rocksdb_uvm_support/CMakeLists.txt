cmake_minimum_required(VERSION 3.21)
project(rocksdb_uvm_support)

set(CMAKE_CXX_STANDARD 17)
find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB


add_executable(rocksdb_uvm_support main.cpp DB/Blockchain_db.cpp DB/Blockchain_db.h error_handling/Result.h)
target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)