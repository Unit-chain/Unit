cmake_minimum_required(VERSION 3.21)
project(vm_test_implementation)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++17")

add_executable(vm_test_implementation main.cpp UVM/VM.h UVM/OPCODES/Opcodes.h UVM/DataTypes/Object.h UVM/DataTypes/u128.h UVM/DataTypes/u256.h UVM/DataTypes/u64.h UVM/DataTypes/i32.h UVM/DataTypes/boolean.h UVM/DataTypes/d64.h UVM/DataTypes/str.h UVM/DataTypes/collection.h UVM/DataTypes/Pools/UniqueConstantPool.h UVM/DataTypes/Pools/SharedConstantPool.h UVM/DataTypes/PrecompiledFunction.h UVM/VMHandler.h timeTest/LogDuration.h)
target_link_libraries(${PROJECT_NAME} Boost::boost)