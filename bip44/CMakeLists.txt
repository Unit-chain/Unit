cmake_minimum_required(VERSION 3.24)
project(bip44)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME}
        utils.hpp
        BIP44.hpp
        BIP39.hpp
        BIP32.hpp
        SHA256.cpp
        SHA256.h
        base58.hpp
        addresses.hpp
        ecdsa.hpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME}
        OpenSSL::SSL)
