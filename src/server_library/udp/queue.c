//
// Created by Kirill Zhukov on 21.04.2023.
//
#include "queue.h"

void queue_init(queue_t *q, int capacity) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->capacity = capacity;
    q->messages = malloc(sizeof(message_t) * capacity);
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void enqueue(queue_t *q, message_t msg) {
    pthread_mutex_lock(&q->lock);
    while (q->size == q->capacity) {
        // resize the queue if there is no more capacity left
        q->capacity *= 2;
        q->messages = realloc(q->messages, sizeof(message_t) * q->capacity);
    }
    q->messages[q->rear] = msg;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

message_t dequeue(queue_t *q) {
    pthread_mutex_lock(&q->lock);
    while (q->size == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    message_t msg = q->messages[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->size--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return msg;
}
