#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#define PORT 8080     // Server listens on this port
#define BUF_SIZE 1024 // size of the read/write buffer

int main()
{
    int server_fd;

    // Create a TCP socket using IPv4. (AF_INET) and stream sockets (SOCK_STEAM). For UDP, it will be SOCK_DGRAM
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket failed!");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure with the server address info
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));    // Clear the memory to avoid garbage
    addr.sin_family = AF_INET;         // IPv4
    addr.sin_addr.s_addr = INADDR_ANY; // Accept connection on any local IP
    addr.sin_port = htons(PORT);       // Convert port to network byte order

    // Bind the socket to the IP address and port
    int bind_ret = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)); // typecast the sockaddr_in addr to the generic `sockaddr` struct which bind expects
    if (bind_ret < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Inform the OS that the server is not going to connect to anyone. It wants to receive connection requests from clients. Please put the server in listen mode
    if (listen(server_fd, 3) < 0)
    {
        perror("Couldn't listen!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0)
        {
            printf("accept failed\n");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            close(server_fd); // client doesn't need the listening socket

            // convert and print the client IP and port
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            char client_name[256];
            snprintf(client_name, sizeof(client_name), "[PID:%d|%s:%d]", getpid(), client_ip, ntohs(client_addr.sin_port));
            printf("Forced child process for the client: %s\n", client_name);

            // Client has connected. Liste to what client has to say
            char buffer[BUF_SIZE] = {0};

            while (1)
            {
                memset(buffer, 0, BUF_SIZE);
                ssize_t bytes = read(client_fd, buffer, BUF_SIZE);
                if (bytes == 0)
                {
                    printf("Client has closed the connection\n");
                    break;
                }
                if (bytes < 0)
                {
                    perror("Read error");
                    break;
                }

                printf("Received from client:%s %s\n", client_name, buffer);
                // Respond to the client, eg: echo back the client's request
                write(client_fd, buffer, strlen(buffer));
            }

            // close the sessions
            close(client_fd);
            printf("Finished sesion with the client: %s\n", client_name);
            exit(0);
        }
        else if (pid > 0)
        {
            // Parent Process
            close(client_fd); // Parent doesn't use the client socket
            printf("Parent continuing with PID: %d\n", getpid());
        }
        else
        {
            perror("fork failed");
            close(client_fd);
        }
    }

    close(server_fd);

    return 0;
}