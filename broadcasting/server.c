#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 8888
#define BROADCAST_IP "255.255.255.255"
#define MESSAGE "Hello, Network!"

int main() {
    int sockfd;
    struct sockaddr_in broadcast_addr;
    int broadcast_permission = 1;
    int send_result;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow broadcast
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast_permission, sizeof(broadcast_permission)) < 0) {
        perror("setsockopt() failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Set up the broadcast address structure
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
    broadcast_addr.sin_port = htons(BROADCAST_PORT);

    // Broadcast the message
    send_result = sendto(sockfd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *) &broadcast_addr, sizeof(broadcast_addr));
    if (send_result < 0) {
        perror("sendto() failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Broadcast message sent: %s\n", MESSAGE);

    // Close the socket
    close(sockfd);
    return 0;
}
