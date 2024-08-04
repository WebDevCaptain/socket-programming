#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// #define SERVER "smtp.example.com"
#define SERVER "127.0.0.1"
#define PORT 8000
// #define PORT 25
#define BUFSIZE 1024

void send_cmd(int sockfd, const char *cmd, const char *arg) {
    char buffer[BUFSIZE];
    if (arg)
        snprintf(buffer, BUFSIZE, "%s %s\r\n", cmd, arg);
    else
        snprintf(buffer, BUFSIZE, "%s\r\n", cmd);

    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFSIZE, 0);
    printf("%s", buffer);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Read server's greeting
    recv(sockfd, buffer, BUFSIZE, 0);
    printf("%s", buffer);

    // HELO command
    send_cmd(sockfd, "HELO", "example.com");

    // MAIL FROM command
    send_cmd(sockfd, "MAIL FROM:", "<sender@example.com>");

    // RCPT TO command
    send_cmd(sockfd, "RCPT TO:", "<recipient@example.com>");

    // DATA command
    send_cmd(sockfd, "DATA", NULL);

    // Email content
    snprintf(buffer, BUFSIZE, "Subject: Test Email\r\n\r\nThis is a test email.\r\n.\r\n");
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFSIZE, 0);
    printf("%s", buffer);

    // QUIT command
    send_cmd(sockfd, "QUIT", NULL);

    // Close the socket
    close(sockfd);

    return 0;
}
