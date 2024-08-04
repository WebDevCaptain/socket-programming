#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// #define PORT 25
#define PORT 8000
#define BUFSIZE 1024
#define MAX_CLIENTS 5

void handle_client(int client_sock) {
    char buffer[BUFSIZE];
    const char *greeting = "220 Welcome to Simple SMTP Server\r\n";
    const char *ok_response = "250 OK\r\n";
    const char *data_prompt = "354 End data with <CR><LF>.<CR><LF>\r\n";
    const char *quit_response = "221 Bye\r\n";

    send(client_sock, greeting, strlen(greeting), 0);

    while (recv(client_sock, buffer, BUFSIZE, 0) > 0) {
        printf("Received: %s", buffer);

        if (strncmp(buffer, "HELO", 4) == 0) {
            send(client_sock, ok_response, strlen(ok_response), 0);
        } else if (strncmp(buffer, "MAIL FROM:", 10) == 0) {
            send(client_sock, ok_response, strlen(ok_response), 0);
        } else if (strncmp(buffer, "RCPT TO:", 8) == 0) {
            send(client_sock, ok_response, strlen(ok_response), 0);
        } else if (strncmp(buffer, "DATA", 4) == 0) {
            send(client_sock, data_prompt, strlen(data_prompt), 0);
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            send(client_sock, quit_response, strlen(quit_response), 0);
            break;
        } else if (buffer[0] == '.') {
            send(client_sock, ok_response, strlen(ok_response), 0);
        }
    }

    close(client_sock);
    printf("Client disconnected\n");
}

void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);
    handle_client(client_sock);
    pthread_exit(NULL);
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id[MAX_CLIENTS];
    int client_count = 0;

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (client_count < MAX_CLIENTS) {
        // Accept incoming connection
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        // Allocate memory for the new socket and create a thread for the client
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        if (pthread_create(&thread_id[client_count], NULL, client_handler, (void *)new_sock) != 0) {
            perror("Thread creation failed");
            close(client_sock);
            free(new_sock);
        } else {
            client_count++;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < client_count; i++) {
        pthread_join(thread_id[i], NULL);
    }

    close(server_sock);
    return 0;
}
