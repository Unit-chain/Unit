//
// Created by Kirill Zhukov on 23.04.2023.
//
#include "udp_server.h"

void start_udp_server(const char *ip_address, int port, int is_ipv6, error_handler err_handler) {
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
    while (1) {
        int pollres = poll(fds, 1, -1);
        if (pollres < 0) {
            err_handler("poll failed");
            exit(EXIT_FAILURE);
        }
//        if ((fds[0].revents & POLLIN) == POLLIN) {
//            puts("Data come");
//        }
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
            printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &clientaddr6.sin6_addr, str, INET6_ADDRSTRLEN),
                   ntohs(clientaddr6.sin6_port));
        } else {
            clientaddrlen = sizeof(clientaddr);
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &clientaddrlen);
            if (n == -1) {
                err_handler("recvfrom failed");
                exit(1);
            }
            printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }
        printf("Data: %s\n", buffer);
    }
    close(sockfd);
}
