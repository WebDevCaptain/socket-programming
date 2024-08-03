#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding to the socket address failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on PORT %d\n", PORT);

    while (1) {
        // Receive msg from client
        int n = recvfrom(server_fd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr*)&client_addr, &addr_len);
        buffer[n] = '\0';
        printf("Received from client: %s \n", buffer);

        // Echo the msg back to client
        sendto(server_fd, buffer, n, MSG_CONFIRM, (const struct sockaddr *)&client_addr, addr_len);
        printf("Echoed messaged back to the client \n");
    }

    // Close the socket
    close(server_fd);
    printf("Shutting down the UDP server");

    return 0;
}