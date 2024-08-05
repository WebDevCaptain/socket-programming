#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};
    socklen_t addr_len = sizeof(client_addr);

    // Create TCP streaming socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create TCP socket.");
        exit(EXIT_FAILURE);
    }

    // Set server addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding socket to the address failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on PORT %d \n", PORT);

    // Accept a client connection
    if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected \n");

    // Read data from the client
    int bytes_read = read(client_fd, buffer, BUFFER_SIZE);

    if (bytes_read > 0) {
        printf("Received data from client: %s \n", buffer);
    }

    const char* resp = "Hello from a TCP server !!!!";

    send(client_fd, resp, strlen(resp), 0);
    printf("Response sent to the client \n");

    // Close the client socket
    close(client_fd);
    printf("Client disconnected \n");

    // Close the server socket
    close(server_fd);
    printf("Server shutdown \n");

    return 0;
}