cmake_minimum_required(VERSION 3.24)
project(libdevcore)
set(CMAKE_CXX_STANDARD 20)

include_directories(
        crypto
        datastructures
        db
        pointers
        utils
        )

add_library(${PROJECT_NAME}
            crypto/Hex.h
            crypto/HMAC_512/HMAC_512.h
            crypto/HMAC_512/HMAC_512.cpp
            crypto/keccak/kec256.h
            crypto/keccak/kec256.cpp
            crypto/SHA3/sha3.h
            crypto/SHA3/sha3.cpp
            crypto/SHA512/SHA512.h
            crypto/SHA512/SHA512.cpp
            datastructures/account/AbstractAccount.h
            datastructures/account/WalletAccount.h
            datastructures/account/Token.h
            datastructures/blockchain/block/Block.h
            datastructures/blockchain/block/BlockBuilder.h
            datastructures/blockchain/block/BlockHeader.h
            datastructures/blockchain/block/Shard.h
            datastructures/blockchain/transaction/Transaction.h
            datastructures/concurrency/DBWriter.h
            datastructures/concurrency/ThreadPool.h
            datastructures/containers/list.h
            datastructures/containers/vector.h
            datastructures/trees/MerkleTree.h
            datastructures/trees/MerkleTree.cpp
            db/DB.h
            db/DBError.h
            db/JsonMergeOperator.h
            pointers/lazy_pointer.h
            utils/StringUtil.h
            utils/StringUtil.cpp
        )

add_subdirectory(bip44)
target_link_libraries(${PROJECT_NAME}
        bip44
        )

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
        find_package(Boost REQUIRED)
        target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
        target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(WIN)
        # do for windows compilation
endif()

