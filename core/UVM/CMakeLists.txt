cmake_minimum_required(VERSION 3.21)
project(UVM)

set(CMAKE_CXX_STANDARD 17)

#include_directories("/usr/local/lib")
find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB
find_package(nlohmann_json 3.2.0 REQUIRED) # adding library for JSON

add_executable(UVM main.cpp VM.cpp VM.h TXManager.cpp TXManager.h Opcodes.h error_handling/Result.h Blockchain_core/Block.cpp Blockchain_core/Block.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h ENV/env.h Blockchain_core/Hex.h Blockchain_core/DB/Unit_CF.h Blockchain_core/DB/Blockchain_db.cpp Blockchain_core/DB/Blockchain_db.h)
target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
#add_executable(main main.cpp error_handling/Result.h Blockchain_core/Block.cpp Blockchain_core/Block.h ENV/env.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h Blockchain_core/Hex.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h)

