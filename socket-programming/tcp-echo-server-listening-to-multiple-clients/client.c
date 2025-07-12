#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"
#define BUF_SIZE 1024

int main()
{
    // socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("client couldn't create socket");
        exit(EXIT_FAILURE);
    }

    // prepare the server details to connect to
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    // convert human readable IP to binary format
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid server address to connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // connect
    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Couldn't connect to server");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUF_SIZE] = {0};
    char input[BUF_SIZE] = {0};
    while (1)
    {
        // write
        printf("Enter the message to send to server (type 'exit' to quit):\n");
        if (fgets(input, BUF_SIZE, stdin) == NULL)
        {
            printf("Error or EOF while reading input\n");
            break;
        }
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0)
        {
            printf("Client exiting...\n");
            break;
        }

        write(client_fd, input, strlen(input));

        // read
        memset(buffer, 0, BUF_SIZE);
        read(client_fd, buffer, sizeof(buffer));
        printf("Server replied: %s\n", buffer);
    }

    // close
    close(client_fd);
    return 0;
}