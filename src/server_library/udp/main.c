#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "udp_server.h"

#if 0
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "8080"
#define CLIENT_PORT "8081"
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void start_udp_server(const char* ip_address, int port, int is_ipv6) {
    // Create socket
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

    // Bind socket to address and port
    struct sockaddr_in6 servaddr6;
    struct sockaddr_in servaddr;
    memset(&servaddr6, 0, sizeof(servaddr6));
    memset(&servaddr, 0, sizeof(servaddr));

    if (is_ipv6) {
        servaddr6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip_address, &servaddr6.sin6_addr);
        servaddr6.sin6_port = htons(port);
        if (bind(sockfd, (struct sockaddr*) &servaddr6, sizeof(servaddr6)) == -1) {
            perror("bind");
            exit(1);
        }
    } else {
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, ip_address, &servaddr.sin_addr);
        servaddr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
            perror("bind");
            exit(1);
        }
    }

    // Receive and print incoming packets
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in6 clientaddr6;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen;
    while (1) {
        memset(&clientaddr6, 0, sizeof(clientaddr6));
        memset(&clientaddr, 0, sizeof(clientaddr));
        memset(buffer, 0, MAX_BUFFER_SIZE);
        if (is_ipv6) {
            clientaddrlen = sizeof(clientaddr6);
            char str[INET6_ADDRSTRLEN]; // Define the char array to store the IP address string
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*) &clientaddr6, &clientaddrlen);
            if (n == -1) {
                perror("recvfrom");
                exit(1);
            }
            printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &clientaddr6.sin6_addr, str, INET6_ADDRSTRLEN), ntohs(clientaddr6.sin6_port));
        } else {
            clientaddrlen = sizeof(clientaddr);
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*) &clientaddr, &clientaddrlen);
            if (n == -1) {
                perror("recvfrom");
                exit(1);
            }
            printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }
        printf("Data: %s\n", buffer);
    }

    // Close socket
    close(sockfd);
}

int main() {
    // Start UDP server on IPv4
    start_udp_server("0.0.0.0", 8080, 0);
    return 0;
}

void start_udp_server(const char* ip_address, int port, int is_ipv6) {
    // Create socket
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

    // Bind socket to address and port
    struct sockaddr_in6 servaddr6;
    struct sockaddr_in servaddr;
    memset(&servaddr6, 0, sizeof(servaddr6));
    memset(&servaddr, 0, sizeof(servaddr));

    if (is_ipv6) {
        servaddr6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip_address, &servaddr6.sin6_addr);
        servaddr6.sin6_port = htons(port);
        if (bind(sockfd, (struct sockaddr*) &servaddr6, sizeof(servaddr6)) == -1) {
            perror("bind");
            exit(1);
        }
    } else {
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, ip_address, &servaddr.sin_addr);
        servaddr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
            perror("bind");
            exit(1);
        }
    }

    // Receive and print incoming packets
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
            perror("poll failed");
            exit(EXIT_FAILURE);
        }
        if ((fds[0].revents & POLLIN) == POLLIN) {
            puts("Data come");
        }
        memset(&clientaddr6, 0, sizeof(clientaddr6));
        memset(&clientaddr, 0, sizeof(clientaddr));
        memset(buffer, 0, MAX_BUFFER_SIZE);
        if (is_ipv6) {
            clientaddrlen = sizeof(clientaddr6);
            char str[INET6_ADDRSTRLEN]; // Define the char array to store the IP address string
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*) &clientaddr6, &clientaddrlen);
            if (n == -1) {
                perror("recvfrom");
                exit(1);
            }
            printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &clientaddr6.sin6_addr, str, INET6_ADDRSTRLEN), ntohs(clientaddr6.sin6_port));
        } else {
            clientaddrlen = sizeof(clientaddr);
            int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*) &clientaddr, &clientaddrlen);
            if (n == -1) {
                perror("recvfrom");
                exit(1);
            }
            printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }
        printf("Data: %s\n", buffer);
    }

    // Close socket
    close(sockfd);
}
#endif

void handler(char *err) {
    size_t sz = strlen(err);
    err[sz] = '\n';
    puts(err);
}

int main() {
    // Start UDP server on IPv4
    start_udp_server("0.0.0.0", 8080, 0, &handler);
    return 0;
}
