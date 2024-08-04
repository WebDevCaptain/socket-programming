#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define CHUNK_SIZE 1024
#define DATA "This is a test message to demonstrate reliable data transmission."

unsigned int calculate_checksum(char *data, size_t len) {
    unsigned int checksum = 0;
    for (size_t i = 0; i < len; ++i) {
        checksum += data[i];
    }
    return checksum;
}

void send_data(int sockfd, char *data, size_t len) {
    size_t total_sent = 0;
    size_t chunk_size = CHUNK_SIZE;
    unsigned int checksum;
    char ack[10];

    while (total_sent < len) {
        size_t remaining = len - total_sent;
        if (remaining < CHUNK_SIZE) {
            chunk_size = remaining;
        }

        checksum = calculate_checksum(data + total_sent, chunk_size);
        send(sockfd, &chunk_size, sizeof(size_t), 0);
        send(sockfd, &checksum, sizeof(unsigned int), 0);
        send(sockfd, data + total_sent, chunk_size, 0);

        recv(sockfd, ack, sizeof(ack), 0);
        if (strcmp(ack, "ACK") != 0) {
            printf("Resending chunk from %zu to %zu\n", total_sent, total_sent + chunk_size);
            continue;
        }

        total_sent += chunk_size;
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    send_data(sockfd, DATA, strlen(DATA));

    close(sockfd);
    return 0;
}
