//
// Created by Kirill Zhukov on 21.04.2023.
//

#ifndef UDP_UDP_PACKAGE_H
#define UDP_UDP_PACKAGE_H
#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(push, 1)
/* Define the packet struct */
typedef struct {
    int seq_num;        /* Sequence number of the packet */
    int data_len;       /* Length of the data in the packet */
    char data[1024];    /* Data to be sent in the packet */
    int checksum;       /* Checksum value of the packet */
} packet_t;

/* Define the request struct */
typedef struct {
    int seq_num;        /* Sequence number of the missing or corrupted packet */
} request_t;

/* Define the acknowledgement struct */
typedef struct {
    int seq_num;        /* Sequence number of the packet being acknowledged */
} ack_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
#endif //UDP_UDP_PACKAGE_H
