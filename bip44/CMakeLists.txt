cmake_minimum_required(VERSION 3.15)
project(BIP39)

set(CMAKE_CXX_STANDARD 17)
add_executable(BIP39 main.cpp BIP44.hpp BIP39.hpp BIP32.hpp SHA256.cpp SHA256.h base58.hpp addresses.hpp)
find_package(OpenSSL REQUIRED)
target_link_libraries(BIP39 OpenSSL::SSL)