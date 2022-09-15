cmake_minimum_required(VERSION 3.21)
project(UVM)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif(MSVC OR MSYS OR MINGW AND NOT APPLE)
    # for detecting Windows compilers
    set(WIN TRUE)
else()
    set(APPLE TRUE)
endif()

add_executable(${PROJECT_NAME} main.cpp BlockHandler.cpp BlockHandler.h Opcodes.h Blockchain_core/Block.cpp Blockchain_core/Block.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/Keccak/kec256.cpp Blockchain_core/Crypto/Keccak/kec256.h ENV/env.h Blockchain_core/Hex.h Blockchain_core/Wallet/WalletAccount.cpp Blockchain_core/Wallet/WalletAccount.h Blockchain_core/Token/Token.cpp Blockchain_core/Token/Token.h Server/Server.cpp Server/Server.h Blockchain_core/DB/DB.cpp Blockchain_core/DB/DB.h Blockchain_core/DB/JSON_merger/JsonMergeOperator.cpp Blockchain_core/DB/JSON_merger/JsonMergeOperator.h Blockchain_core/Crypto/SHA512/SHA512.cpp Blockchain_core/Crypto/SHA512/SHA512.h Blockchain_core/Crypto/HMAC_512/HMAC_512.cpp Blockchain_core/Crypto/HMAC_512/HMAC_512.h Blockchain_core/Merkle/MerkleTree.cpp Blockchain_core/Merkle/MerkleTree.h Blockchain_core/Crypto/SHA3/sha3.cpp Blockchain_core/Crypto/SHA3/sha3.h containers/list.h)

if(LINUX)
    message(STATUS ">>> Linux found")
    find_package(Boost)
    include_directories(/usr/local/lib) # need to find rocksdb lib
    set(ROCKSDB_SHARED_LIB /usr/local/lib/librocksdb.so)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    target_link_libraries(${PROJECT_NAME} ${ROCKSDB_SHARED_LIB})
    target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(APPLE)
    find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB
    find_package(Boost)
    target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
    target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(WIN)
    # do for windows compilation
endif()


#add_subdirectory(external/leveldb)
#target_link_libraries(${PROJECT_NAME} nlohmann_json)
#add_executable(main main.cpp error_handling/Result.h Blockchain_core/Block.cpp Blockchain_core/Block.h ENV/env.h Blockchain_core/Transaction.cpp Blockchain_core/Transaction.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h Blockchain_core/Hex.h Blockchain_core/Crypto/kec256.cpp Blockchain_core/Crypto/kec256.h)
#add_executable(untitled11 main.cpp)
#target_link_libraries(${PROJECT_NAME} Boost::boost)
