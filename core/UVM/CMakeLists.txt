cmake_minimum_required(VERSION 3.21)
project(UVM)

set(CMAKE_CXX_STANDARD 17)



#include_directories("/usr/local/lib")
find_package(RocksDB)

add_executable(UVM main.cpp VM.cpp VM.h TXManager.cpp TXManager.h Opcodes.h)

#target_link_libraries(UVM
#        ${ROCKSDB_LIB})
target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)

add_executable(main main.cpp)

