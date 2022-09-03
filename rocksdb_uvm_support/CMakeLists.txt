cmake_minimum_required(VERSION 3.21)
project(rocksdb_uvm_support)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

add_executable(rocksdb_uvm_support main.cpp DB/DB.cpp DB/DB.h error_handling/Result.h)

if(LINUX)
    message(STATUS ">>> Linux found")
    include_directories(/usr/local/lib) # need to find rocksdb lib
    set(ROCKSDB_SHARED_LIB /usr/local/lib/librocksdb.so)
    target_link_libraries(${PROJECT_NAME} ${ROCKSDB_SHARED_LIB})
elseif(APPLE)
    find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB
    find_package(nlohmann_json 3.2.0 REQUIRED) # adding library for JSON
    target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
endif()
