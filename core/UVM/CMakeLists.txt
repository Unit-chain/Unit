cmake_minimum_required(VERSION 3.21)
project(UVM)

set(CMAKE_CXX_STANDARD 17)

add_executable(UVM main.cpp VM.cpp VM.h TXManager.cpp TXManager.h Opcodes.h)