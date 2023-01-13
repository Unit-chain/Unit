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

include_directories(libdevcore)
add_subdirectory(libdevcore)
add_executable(${PROJECT_NAME}
        main.cpp
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
        pools/TransactionPoolErrors.h
        blockHandler/ShardInserter.h
        UnitInitiator.h
        pools/PendingPool.h
        )

target_link_libraries(${PROJECT_NAME} libdevcore)
include_directories(
        pools/
        global/
        global/errors
        libdevcore/
        libdevcore/bip44/
        libdevcore/crypto/
        libdevcore/datastructures/
        libdevcore/datastructures/blockchain/
)

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
#    target_link_libraries(${PROJECT_NAME} LibreSSL::SSL)
    find_package(RocksDB REQUIRED) # add rocksdb library to interact with RocksDB
    find_package(Boost REQUIRED)
    target_link_libraries(${PROJECT_NAME} RocksDB::rocksdb)
    target_link_libraries(${PROJECT_NAME} Boost::boost)
elseif(WIN)
    # do for windows compilation
endif()