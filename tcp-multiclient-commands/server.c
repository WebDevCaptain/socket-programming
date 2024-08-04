#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 8080
#define BUF_SIZE 1024
#define DAILY_WORD_LIMIT 1000

int total_words_today = 0;

void handle_client(int client_socket) {
    char buffer[BUF_SIZE];
    int words_today = 0;
    
    // Communicate with the client
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        ssize_t bytes_received = recv(client_socket, buffer, BUF_SIZE - 1, 0);
        if (bytes_received <= 0) {
            perror("recv failed or client disconnected");
            close(client_socket);
            return;
        }

        buffer[bytes_received] = '\0';
        
        if (strcmp(buffer, "start") == 0) {
            // Start communication
            char response[] = "You can now send messages. Type 'stop' to end.";
            send(client_socket, response, strlen(response), 0);
        } else if (strcmp(buffer, "stop") == 0) {
            // Stop communication
            char response[] = "Goodbye!";
            send(client_socket, response, strlen(response), 0);
            break;
        } else {
            // Handle message
            int word_count = 0;
            char *token = strtok(buffer, " \n");
            while (token != NULL) {
                word_count++;
                token = strtok(NULL, " \n");
            }
            
            if (total_words_today + word_count > DAILY_WORD_LIMIT) {
                char response[] = "Daily word limit exceeded.";
                send(client_socket, response, strlen(response), 0);
            } else {
                total_words_today += word_count;
                words_today += word_count;
                // Echo the message back to the client
                send(client_socket, buffer, strlen(buffer), 0);
            }
        }
    }
    
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Accept a new connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Create a new process to handle the client
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(client_socket);
        } else if (pid == 0) {
            // Child process
            close(server_fd); // Child doesn't need the listener
            handle_client(client_socket);
            exit(0);
        } else {
            // Parent process
            close(client_socket); // Parent doesn't need this
        }
    }

    close(server_fd);
    return 0;
}
