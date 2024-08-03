#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char* message = "Hey! how's it going ?";
    socklen_t addr_len = sizeof(server_addr);

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address already occupied");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send message to server
    sendto(sock, message, strlen(message), MSG_CONFIRM, (const struct sockaddr*)&server_addr, addr_len);
    printf("Message sent to the server: %s \n", message);

    // Receive response (echoed)
    int n = recvfrom(sock, (char*)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&server_addr, &addr_len);
    buffer[n] = '\0';
    printf("Received message from server: %s \n", buffer);

    // Verify if received message matches sent message
    if (strcmp(message, buffer) == 0) {
        printf("Message verified \n");
    } else {
        printf("Verification failed. '%s' does not match '%s' \n",message, buffer);
    }

    // Close the socket
    shutdown(sock, SHUT_RDWR);
    close(sock);

    printf("Connection closed from client side, socket destroyed \n");

    return 0;
}