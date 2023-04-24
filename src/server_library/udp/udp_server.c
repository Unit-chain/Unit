//
// Created by Kirill Zhukov on 23.04.2023.
//
#include "udp_server.h"

atomic_ullong time_counter;

_Noreturn void start_udp_server(const char *ip_address, int port, int is_ipv6, error_handler err_handler) {
    int sockfd;
    if (is_ipv6) {
        sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    } else {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in6 servaddr6;
    struct sockaddr_in servaddr;
    memset(&servaddr6, 0, sizeof(servaddr6));
    memset(&servaddr, 0, sizeof(servaddr));

    if (is_ipv6) {
        servaddr6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip_address, &servaddr6.sin6_addr);
        servaddr6.sin6_port = htons(port);
        if (bind(sockfd, (struct sockaddr *) &servaddr6, sizeof(servaddr6)) == -1) {
            err_handler("bind error");
            exit(1);
        }
    } else {
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, ip_address, &servaddr.sin_addr);
        servaddr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
            err_handler("bind error");
            exit(1);
        }
    }

    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in6 clientaddr6;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen;
    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    queue_t q;
    queue_init(&q);
    start_threads(get_num_cpus(), &q);
#if 1
    uint64_t packages_received = 0;
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

    while (1) {
        memset(&clientaddr6, 0, sizeof(clientaddr6));
        memset(&clientaddr, 0, sizeof(clientaddr));
        memset(buffer, 0, MAX_BUFFER_SIZE);
        if (is_ipv6) {
            clientaddrlen = sizeof(clientaddr6);
            char str[INET6_ADDRSTRLEN]; // Define the char array to store the IP address string
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr6, &clientaddrlen);
            if (n == -1) {
                err_handler("recvfrom failed");
                exit(1);
            }
#if 1
            packages_received++;
#endif
//            printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &clientaddr6.sin6_addr, str, INET6_ADDRSTRLEN), ntohs(clientaddr6.sin6_port));
        } else {
            clientaddrlen = sizeof(clientaddr);
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &clientaddrlen);
            if (n == -1) {
                err_handler("recvfrom failed");
                exit(1);
            }
#if 1
            packages_received++;
#endif
//            printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }
        queue_push(&q, buffer, MAX_BUFFER_SIZE);
#if 1
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        if ((end_time.tv_sec - start_time.tv_sec) > 0) {
            // Calculate packets per second
            long long elapsed_nanos = (end_time.tv_sec - start_time.tv_sec) * SEC_TO_NSEC +
                                      end_time.tv_nsec - start_time.tv_nsec;
            double pps = (double) packages_received / (double) elapsed_nanos * SEC_TO_NSEC;

            // Print result and reset counters
            printf("RPS: %f, packages: %llu\n", pps, packages_received);
            start_time = end_time;
            packages_received = 0;
        }
#endif
    }
    close(sockfd);
}

_Noreturn void *thread_function(void *msg_queue) {
    queue_t *msgs = (queue_t*) msg_queue;
    while(1) {
        if (msgs->count > 0) {
            char* msg = queue_pop(msgs);
            uint64_t val = strtoull(msg, NULL, 10);
            struct timespec time;
            long long ms, sec_ms, nsec_ms;
            clock_gettime(CLOCK_REALTIME, &time);
            sec_ms = time.tv_sec * 1000LL;
            nsec_ms = time.tv_nsec / 1000000LL;
            ms = sec_ms + nsec_ms;
//            printf("time took: %llu ms\n", (ms - val));
            free(msg);
        }
    }
}

void start_threads(int n, queue_t *msh_queue) {
    pthread_t threads[n-2];
    pthread_attr_t attr[n-2];
    int thread_args[n];
    for (int i = 0; i < n-2; i++) {
        thread_args[i] = i;
        pthread_attr_init(&attr[i]);
        pthread_attr_setdetachstate(&attr[i], PTHREAD_CREATE_DETACHED);
        if (pthread_create(&threads[i], &attr[i], thread_function, msh_queue) != 0) {
            fprintf(stderr, "Error: Failed to create thread\n");
            exit(EXIT_FAILURE);
        }
    }
}
