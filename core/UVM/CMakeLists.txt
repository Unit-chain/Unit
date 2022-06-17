cmake_minimum_required(VERSION 3.21)
project(UVM)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
else()
    set(APPLE TRUE)
endif()

add_executable(${PROJECT_NAME} main.cpp VM.cpp VM.h TXManager.cpp TXManager.h Opcodes.h error_handling/Result.h Blockchain_core/Block.cpp Blockchain_core/Block.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h ENV/env.h Blockchain_core/Hex.h Blockchain_core/DB/Blockchain_db.cpp Blockchain_core/DB/Blockchain_db.h Blockchain_core/Wallet/WalletAccount.cpp Blockchain_core/Wallet/WalletAccount.h Blockchain_core/Token/Token.cpp Blockchain_core/Token/Token.h error_handling/Optional.h)

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

#target_link_libraries(${PROJECT_NAME} nlohmann_json)
#add_executable(main main.cpp error_handling/Result.h Blockchain_core/Block.cpp Blockchain_core/Block.h ENV/env.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h Blockchain_core/Hex.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h)
#add_executable(untitled11 main.cpp)
#target_link_libraries(${PROJECT_NAME} Boost::boost)
