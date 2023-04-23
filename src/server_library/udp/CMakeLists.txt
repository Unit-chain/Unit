cmake_minimum_required(VERSION 3.25)
project(udp C)

set(CMAKE_C_STANDARD 17)

add_executable(udp main.c udp_package.h crc32_table.h crc32.h queue.h queue.c)