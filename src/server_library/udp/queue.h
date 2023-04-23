//
// Created by Kirill Zhukov on 21.04.2023.
//

#ifndef UDP_QUEUE_H
#define UDP_QUEUE_H

#include "stdlib.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 4096
#define QUEUE_SIZE 10

typedef struct {
    char buffer[MAX_MESSAGE_SIZE];
    struct sockaddr_in cliaddr;
    socklen_t len;
} message_t;

typedef struct {
    int front;
    int rear;
    int size;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    message_t *messages;
} queue_t;

#ifdef __cplusplus
extern "C" {
#endif

void queue_init(queue_t *q, int capacity);
void queue_destroy(queue_t *q);
void enqueue(queue_t *q, message_t msg);
message_t dequeue(queue_t *q);

#ifdef __cplusplus
}
#endif
#endif //UDP_QUEUE_H
