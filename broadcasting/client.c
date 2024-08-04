#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in listen_addr;
    char buffer[BUFFER_SIZE];
    int recv_len;
    socklen_t addr_len;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set up the listen address structure
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_port = htons(BROADCAST_PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) < 0) {
        perror("bind() failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for broadcast messages on port %d...\n", BROADCAST_PORT);

    // Receive broadcast messages
    addr_len = sizeof(listen_addr);
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &listen_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom() failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        buffer[recv_len] = '\0'; // Null-terminate the received string
        printf("Received broadcast message: %s\n", buffer);
    }

    // Close the socket (unreachable in this loop)
    close(sockfd);
    return 0;
}
