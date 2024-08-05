#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen;
    char buffer[BUF_SIZE];
    char msg = 'Y';

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create UDP socket");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 address from string to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address already occupied");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Send the msg (one byte) to server
    sendto(server_fd, &msg, 1, MSG_CONFIRM, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Sent one byte to the server \n");

    // Receive Acknowledgement (ACK)
    int n = recvfrom(server_fd, (char*)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr*)&client_addr, &addrlen);
    buffer[n] = '\0';

    printf("Received message from server: %s \n", buffer);

    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);

    printf("UDP socket closed from client side\n");

    return 0;
}