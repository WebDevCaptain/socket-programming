#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd = 0, n, i;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("Invalid address or Address not supported \n");
        return -1;
    }

    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. \n");
        return -1;
    }

    printf("Enter the number of elements: ");
    scanf("%d", &n);

    double numbers[n];

    printf("Enter the numbers: ");
    for (int i=0; i<n; i++) {
        scanf("%lf", &numbers[i]);
    }

    write(server_fd, &n, sizeof(int));
    write(server_fd, numbers, sizeof(numbers));

    double product;
    read(server_fd, &product, sizeof(double));

    printf("Product of the numbers: %lf \n", product);

    close(server_fd);

    return 0;
}