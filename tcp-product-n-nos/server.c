#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


void handleClient(int);


int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(server_addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&client_addr, 0, sizeof(client_addr));

    while (1) {
        printf("Server waiting for connection \n");

        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen)) < 0) {
            perror("Accept failed.");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Connection established. \n");

        handleClient(client_fd);
    }

    close(server_fd);

    return 0;
}

void handleClient(int client_fd) {
    int n, i;
    double product = 1.0;
    read(client_fd, &n, sizeof(int));

    double numbers[n];
    read(client_fd, numbers, sizeof(numbers));

    for (int i=0; i<n; i++) {
        product *= numbers[i];
    }

    write(client_fd, &product, sizeof(double));
    close(client_fd);
}