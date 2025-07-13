#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/select.h>
#include <poll.h>

#define PORT 8080     // Server listens on this port
#define BUF_SIZE 1024 // size of the read/write buffer

#define INITIAL_CLIENT_CAP 1024

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

    int opt = 1;
    setsockopt(server_fd,
               SOL_SOCKET,         // at this level
               SO_REUSEADDR,       // allow reuse of local addresses, even if they are in TIME_WAIT state
               &opt, sizeof(opt)); // enable this option

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

    int client_capacity = INITIAL_CLIENT_CAP;
    // int clients[INITIAL_CLIENT_CAP];                 // to track connected clients FDs
    // char client_names[INITIAL_CLIENT_CAP][64] = {0}; // to track connected clients FDs
    // struct pollfd poll_fds[INITIAL_CLIENT_CAP];
    int *clients = malloc(sizeof(int) * client_capacity);
    char (*client_names)[64] = malloc(sizeof(char[64]) * client_capacity);
    struct pollfd *poll_fds = malloc(sizeof(struct pollfd) * client_capacity);

    for (int i = 0; i < client_capacity; i++)
    {
        clients[i] = -1;
        client_names[i][0] = '\0';
    }

    int number_clients_connected = 0;

    while (1)
    {

        int nfds = 0;
        poll_fds[0].fd = server_fd;
        poll_fds[0].events = POLLIN;
        nfds = 1;

        // also watch all connected client sockets
        for (int i = 0; i < client_capacity; i++)
        {
            if (clients[i] != -1)
            {
                poll_fds[nfds].fd = clients[i];
                poll_fds[nfds].events = POLLIN;
                nfds++;
            }
        }

        int ready = poll(poll_fds, nfds, -1); // -1 means blocks immediately

        if (ready < 0)
        {
            perror("poll error");
            break;
        }

        // is a new client trying to connect?
        if (poll_fds[0].revents & POLLIN)
        {
            // Expand capacity if needed
            if (number_clients_connected >= client_capacity - 1)
            {
                int new_capacity = client_capacity * 2;
                clients = realloc(clients, sizeof(int) * new_capacity);
                client_names = realloc(client_names, sizeof(char[64]) * new_capacity);
                poll_fds = realloc(poll_fds, sizeof(struct pollfd) * new_capacity);
                for (int i = client_capacity; i < new_capacity; i++)
                {
                    clients[i] = -1;
                    client_names[i][0] = '\0';
                }
                client_capacity = new_capacity;
                printf("Incresed capacity to %d clients.\n", client_capacity);
            }

            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_fd < 0)
            {
                printf("accept failed\n");
                continue;
            }
            // if (client_fd >= client_capacity)
            // {
            //     printf("client_fd %d exceeds INITIAL_CLIENT_CAP %d - closing\n", client_fd, INITIAL_CLIENT_CAP);
            //     close(client_fd);
            //     continue;
            // }

            // convert and print the client IP and port
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            char client_name[64];
            snprintf(client_name, sizeof(client_name), "[FD:%d|%s:%d]", client_fd, client_ip, ntohs(client_addr.sin_port));
            snprintf(client_names[client_fd], sizeof(client_names[client_fd]), "%s", client_name);
            // add this new client to our client list
            int added = 0; // tracks if the client is added in the select loop or not
            for (int i = 0; i < client_capacity; i++)
            {
                // find an empty slot
                if (clients[i] == -1)
                {
                    clients[i] = client_fd;
                    added = 1;
                    number_clients_connected += 1;
                    printf("Number of clients connected: %d\n", number_clients_connected);
                    printf("New client: connected at FD: %s\n", client_name);
                    break;
                }
            }
            // too many clients already
            if (!added)
            {
                printf("Too many clients. Closing FD %d\n", client_fd);
                close(client_fd);
                client_names[client_fd][0] = '\0';
            }
        }

        // check which clients have sent messages
        for (int i = 1; i < nfds; i++)
        {
            int fd = poll_fds[i].fd;
            if (poll_fds[i].revents & POLLIN)
            {
                // read
                char buffer[BUF_SIZE] = {0};

                // try to read from this client
                ssize_t bytes = read(fd, buffer, BUF_SIZE);

                if (bytes <= 0)
                {
                    // client has disconnected or there was an error
                    printf("Client %s has disconnected\n", client_names[fd]);
                    close(fd);
                    client_names[fd][0] = '\0';
                    for (int j = 0; j < client_capacity; j++)
                    {
                        if (clients[j] == fd)
                        {
                            clients[j] = -1;
                            number_clients_connected -= 1;
                            printf("Number of clients connected: %d\n", number_clients_connected);
                            break;
                        }
                    }
                }
                else
                {
                    // read the data from client and echo it back
                    printf("Received from client:%s %s\n", client_names[fd], buffer);
                    write(fd, buffer, bytes);
                }
            }
        }
    }

    close(server_fd);
    free(clients);
    free(client_names);
    free(poll_fds);

    return 0;
}