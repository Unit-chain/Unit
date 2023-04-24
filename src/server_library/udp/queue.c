//
// Created by Kirill Zhukov on 21.04.2023.
//
#include "queue.h"

void queue_init(queue_t* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void queue_push(queue_t* q, char* data, size_t data_len) {
    node_t* new_node = malloc(sizeof(node_t));
    new_node->data = malloc(data_len + 1);  // +1 for null terminator
    memcpy(new_node->data, data, data_len);
    new_node->data[data_len] = '\0';  // null-terminate the string
    new_node->next = NULL;
    pthread_mutex_lock(&q->lock);
    if (q->tail == NULL) {
        q->head = new_node;
        q->tail = new_node;
    } else {
        q->tail->next = new_node;
        q->tail = new_node;
    }
    q->count++;
    pthread_mutex_unlock(&q->lock);
    pthread_cond_signal(&q->not_empty);
}

char* queue_pop(queue_t* q) {
    pthread_mutex_lock(&q->lock);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    char* data = q->head->data;
    node_t* old_head = q->head;
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->count--;
    pthread_mutex_unlock(&q->lock);
    free(old_head);
    return data;
}

