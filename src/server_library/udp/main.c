#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <netdb.h>

#define PORT "8080"
#define CLIENT_PORT "8081"
#define MAX_MESSAGE_SIZE 1024
#define MAX_CLIENTS 10

void print_address_info(const struct sockaddr_storage *addr) {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    int error;

    // resolve address to human-readable form
    error = getnameinfo((const struct sockaddr *)addr, sizeof(*addr), host, sizeof(host), port, sizeof(port), NI_NUMERICSERV);
    if (error != 0) {
        fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(error));
        return;
    }

    // print host and port
    printf("Address: %s:%s\n", host, port);

    // extract port number
    if (addr->ss_family == AF_INET) {
        const struct sockaddr_in *ipv4 = (const struct sockaddr_in *)addr;
        uint16_t port_number = ntohs(ipv4->sin_port);
        printf("Port number: %hu\n", port_number);
    }
    else if (addr->ss_family == AF_INET6) {
        const struct sockaddr_in6 *ipv6 = (const struct sockaddr_in6 *)addr;
        uint16_t port_number = ntohs(ipv6->sin6_port);
        printf("Port number: %hu\n", port_number);
    }
}

void open_server(char *port, int ipv) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = ipv; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    // Loop through all the results and bind to the first possible
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        // Enable address reuse to avoid "Address already in use" errors
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }
    freeaddrinfo(servinfo);
    if (p == NULL) {
        fprintf(stderr, "Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for messages...\n");
    struct sockaddr_storage client_addr;
    socklen_t len;
//    char ipstr[INET6_ADDRSTRLEN];
    char client_addr_str[
            (ipv == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN
            ];

    struct pollfd fds[MAX_CLIENTS];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    int max_fd = sockfd;
    while (1) {
        int pollres = poll(fds, max_fd + 1, -1);
        if (pollres < 0) {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }
        if ((fds[0].revents & POLLIN) == POLLIN) {
            puts("Data come");
        }

        // reading message
        char buffer[MAX_MESSAGE_SIZE];
        ssize_t numbytes;
        numbytes = recvfrom(sockfd, buffer, MAX_MESSAGE_SIZE - 1, 0, (struct sockaddr *)&client_addr, &len);
        if (numbytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        buffer[numbytes] = '\0';
        printf("Received message: %s\n", buffer);

        print_address_info(&client_addr);
    }
    close(sockfd);
}

int main(void) {
    open_server(PORT, AF_INET);
    return 0;
}