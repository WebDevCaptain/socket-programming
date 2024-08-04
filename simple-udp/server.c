#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024
#define ACK "ACK"

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[BUF_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Cannot create the socket");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding socket to the address
    if (bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding socket address failed. ");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on PORT %d \n", PORT);

    while (1) {
        // Receive a byte from client
        int n = recvfrom(server_fd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr*)&client_addr, &addrlen);
        buffer[n] = '\0';
        printf("Received data from client: %s \n", buffer);

        // Send ACK
        sendto(server_fd, ACK, strlen(ACK), MSG_CONFIRM, (const struct sockaddr*)&client_addr, addrlen);
        printf("Acknowledgement sent\n");
    }

    close(server_fd);
    printf("Shutting down UDP server.");
    return 0;
}