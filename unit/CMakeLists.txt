cmake_minimum_required(VERSION 3.21)
project(unit)

set(CMAKE_CXX_STANDARD 20)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif(MSVC OR MSYS OR MINGW AND NOT APPLE)
    # for detecting Windows compilers
    set(WIN TRUE)
else()
    set(APPLE TRUE)
endif()

add_executable(unit
        main.cpp
        libdevcore/bip44/BIP44.hpp
        libdevcore/bip44/BIP39.hpp
        libdevcore/bip44/BIP32.hpp
        libdevcore/bip44/SHA256.cpp
        libdevcore/bip44/SHA256.h
        libdevcore/bip44/base58.hpp
        libdevcore/bip44/addresses.hpp
        libdevcore/crypto/keccak/kec256.cpp
        libdevcore/crypto/keccak/kec256.h libdevcore/crypto/Hex.h
        libdevcore/datastructures/trees/MerkleTree.h
        libdevcore/datastructures/trees/MerkleTree.cpp
        libdevcore/crypto/SHA3/sha3.h
        libdevcore/crypto/SHA3/sha3.cpp
        libdevcore/bip44/BIP44.hpp
        libdevcore/bip44/BIP39.hpp
        libdevcore/bip44/BIP32.hpp
        libdevcore/bip44/SHA256.cpp
        libdevcore/bip44/SHA256.h
        libdevcore/bip44/ecdsa.hpp
        libdevcore/bip44/base58.hpp
        libdevcore/bip44/addresses.hpp
        libdevcore/bip44/utils.hpp
        libdevcore/datastructures/blockchain/transaction/Transaction.h
        libdevcore/utils/StringUtil.h
        libdevcore/utils/StringUtil.cpp
        libdevcore/pointers/lazy_pointer.h
        libdevcore/datastructures/blockchain/block/Block.h
        libdevcore/datastructures/blockchain/block/Block.h
        libdevcore/datastructures/blockchain/block/BlockHeader.h
        libdevcore/datastructures/blockchain/block/Shard.h
        libdevcore/datastructures/containers/list.h
        libdevcore/datastructures/containers/vector.h
        libdevcore/datastructures/account/WalletAccount.h
        libdevcore/datastructures/account/AbstractAccount.h
        libdevcore/db/JsonMergeOperator.h
        libdevcore/db/DB.h
        libdevcore/db/DBError.h
        libdevcore/datastructures/concurrency/ThreadPool.h
        libdevcore/datastructures/concurrency/DBWriter.h
        libdevcore/datastructures/blockchain/block/BlockBuilder.h
        libdevcore/datastructures/account/Token.h
        global/errors/WalletErrors.h
        global/GlobalVariables.h
        blockHandler/BlockHandler.h
        blockHandler/TransactionProcessor.h
        server/Server.h
        server/RpcMethod.h
        server/ServerIncludes.h
        server/RpcFilterChain.h
        server/RpcExceptions.h
        pools/TransactionPool.h
        pools/TransactionPoolErrors.h blockHandler/ShardInserter.h UnitInitiator.h pools/PendingPool.h)

if(LINUX)
    message(STATUS ">>> Linux found")
    find_package(Boost)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
    include_directories(/usr/local/lib) # need to find rocksdb lib
    set(ROCKSDB_SHARED_LIB /usr/local/lib/librocksdb.so)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    target_link_libraries(${PROJECT_NAME} ${ROCKSDB_SHARED_LIB})
    target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(APPLE)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
    find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB
    find_package(Boost)
    target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
    target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(WIN)
    # do for windows compilation
endif()