#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define CHUNK_SIZE 1024

unsigned int calculate_checksum(char *data, size_t len) {
    unsigned int checksum = 0;
    for (size_t i = 0; i < len; ++i) {
        checksum += data[i];
    }
    return checksum;
}

void receive_data(int client_sock) {
    size_t chunk_size;
    unsigned int checksum, received_checksum;
    char buffer[CHUNK_SIZE];
    char ack[] = "ACK";
    char nack[] = "NACK";
    size_t total_received = 0;

    while (1) {
        if (recv(client_sock, &chunk_size, sizeof(size_t), 0) <= 0) {
            break;
        }
        if (recv(client_sock, &received_checksum, sizeof(unsigned int), 0) <= 0) {
            break;
        }
        if (recv(client_sock, buffer, chunk_size, 0) <= 0) {
            break;
        }

        checksum = calculate_checksum(buffer, chunk_size);

        if (checksum == received_checksum) {
            send(client_sock, ack, sizeof(ack), 0);
            total_received += chunk_size;
            buffer[chunk_size] = '\0';
            printf("Received chunk: %s\n", buffer);
        } else {
            send(client_sock, nack, sizeof(nack), 0);
            printf("Checksum mismatch. Expected: %u, Received: %u. Requesting retransmission.\n", checksum, received_checksum);
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock < 0) {
        perror("Accept failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    receive_data(client_sock);

    close(client_sock);
    close(server_sock);
    return 0;
}
