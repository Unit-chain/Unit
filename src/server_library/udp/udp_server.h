//
// Created by Kirill Zhukov on 23.04.2023.
//

#ifndef UDP_UDP_SERVER_H
#define UDP_UDP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int n;
    pthread_t *threads;
} thread_array;

typedef struct {
    uint64_t data_id;
    char *payload;
} payload_t;

///@brief accept errors from server
typedef void (*error_handler)(char *);

///@brief accept data from server and parse it
///@param char* data accepted by server
///@param void* data handler
typedef void (*data_handler)(char *, void*);

///@brief starting UDP server for receiving data on port
///@param ip_address declaring which ip will be used. For example, to accept all connection should be used "0.0.0.0"
///@param port setting port which will receive data
///@param is_ipv6 variable to choose which ip version will be used. 0 - IPV4, 1 - IPV6
///@param err_handler function pointer to handle errors
void start_udp_server(const char* ip_address, int port, int is_ipv6, error_handler err_handler);

///@brief starting 'n' threads
void start_threads(int n);

#ifdef __cplusplus
}
#endif

#endif //UDP_UDP_SERVER_H
