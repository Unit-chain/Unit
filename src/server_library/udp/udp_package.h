//
// Created by Kirill Zhukov on 21.04.2023.
//

#ifndef UDP_UDP_PACKAGE_H
#define UDP_UDP_PACKAGE_H
#include "unistd.h"
#include "stdint.h"

#define SEC_TO_NSEC 1000000000L

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
///@brief packet_t struct used to transfer data inside protocol
///@param version protocol version
///@param data_id data id used for building data
///@param pkg_num number of package in data stream
///@param crc32 checksum used for checking if data corrupted
///@param prev_pkg_sz previous package size used for data plug
///@param pld_sz represents payload size
typedef struct {
    uint16_t version;
    uint64_t data_id;
    uint16_t pkg_num;
    uint32_t crc32;
    uint32_t prev_pkg_sz;
    uint32_t pld_sz;
    uint8_t terminal;
    char *payload[];
} packet_t;
#pragma pack(pop)


#ifdef __cplusplus
}
#endif
#endif //UDP_UDP_PACKAGE_H
