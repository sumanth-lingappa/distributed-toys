#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/select.h>

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

    int clients[FD_SETSIZE];                 // to track connected clients FDs
    char client_names[FD_SETSIZE][64] = {0}; // to track connected clients FDs

    for (int i = 0; i < FD_SETSIZE; i++)
    {
        clients[i] = -1;
    }

    fd_set read_fds;        // The set of sockets we are watching for "read" activity
    int max_fd = server_fd; // we will need to tell select() the hightest-numbered FD
    int number_clients_connected = 0;

    while (1)
    {
        FD_ZERO(&read_fds);           // clear the FD set, then all sockets we care about
        FD_SET(server_fd, &read_fds); // Always wath the listening socket

        // also watch all connected client sockets
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (clients[i] != -1)
            {
                FD_SET(clients[i], &read_fds);
            }
        }
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        // wait for something to happen
        // blocks until at least one socket is ready
        int ready = select(max_fd + 1, // highest number of FDs to keep track (0 to max_fd)
                           &read_fds,  // set of FDs to monitor for reading
                           NULL,       // set of FDs to monitor for writing
                           NULL,       // set of FDs to monitor for exception events (rare)
                           //    &timeout);  // How long to wait before giving up
                           NULL);      // How long to wait before giving up

        if (ready == 0)
        {
            printf("Noting happened in this round\n");
            break;
        }
        if (ready < 0)
        {
            perror("select error");
            break;
        }

        // is a new client trying to connect?
        if (FD_ISSET(server_fd, &read_fds))
        {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_fd < 0)
            {
                printf("accept failed\n");
                continue;
            }
            if (client_fd >= FD_SETSIZE)
            {
                printf("client_fd %d exceeds FD_SETSIZE %d - closing\n", client_fd, FD_SETSIZE);
                close(client_fd);
                continue;
            }

            // convert and print the client IP and port
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            char client_name[64];
            snprintf(client_name, sizeof(client_name), "[FD:%d|%s:%d]", client_fd, client_ip, ntohs(client_addr.sin_port));
            snprintf(client_names[client_fd], sizeof(client_names[client_fd]), "%s", client_name);
            // add this new client to our client list
            int added = 0; // tracks if the client is added in the select loop or not
            for (int i = 0; i < FD_SETSIZE; i++)
            {
                // find an empty slot
                if (clients[i] == -1)
                {
                    clients[i] = client_fd;
                    if (client_fd > max_fd)
                    {
                        max_fd = client_fd; // update max_fd for select()
                    }
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
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            int fd = clients[i];
            if (fd != -1 && FD_ISSET(fd, &read_fds))
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
                    clients[i] = -1;
                    number_clients_connected -= 1;
                    printf("Number of clients connected: %d\n", number_clients_connected);
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

    return 0;
}