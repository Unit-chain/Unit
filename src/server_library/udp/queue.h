//
// Created by Kirill Zhukov on 21.04.2023.
//

#ifndef UDP_QUEUE_H
#define UDP_QUEUE_H

#include "stdlib.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

typedef struct node {
    char* data;
    struct node* next;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} queue_t;

void queue_init(queue_t* q);
void queue_push(queue_t* q, char* data,  size_t data_len);
char* queue_pop(queue_t* q);

#ifdef __cplusplus
}
#endif
#endif //UDP_QUEUE_H
