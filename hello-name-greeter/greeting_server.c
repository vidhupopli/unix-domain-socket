#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Two constants are created.
#define SOCKET_PATH "/tmp/greeting.sock"
#define BUF_SIZE 256

int main()
{
    // 0. Variables we need and cleanup
    int master_socket_fd, client_specific_conn_socket_fd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];  // Creating memory / character array
    unlink(SOCKET_PATH); // Remove old socket file if exists

    // 1. Create socket
    master_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0); // We want unix domain socket of type stream
    if (master_socket_fd < 0)
    {
        perror("socket");
        exit(1); // Server stops.
    }
    printf("Master socket fd: %d\n", master_socket_fd);

    // 2. Prepare address for binding then binding
    memset(&addr, 0, sizeof(addr));                                 // Initting memory by setting 0 to the struct
    addr.sun_family = AF_UNIX;                                      // setting a number
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); // setting path name
    printf("socket type: %d\n", addr.sun_family);                   // 1 is UNIX Domain Socket
    printf("socket path: %s\n", addr.sun_path);
    if (bind(master_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    printf("socket binding to filesystem node succeeded\n");

    // 3. Listen (marking the socket as passive aka ready for listening to connections)
    // Also sets up connection queue.
    if (listen(master_socket_fd, 5) < 0)
    {
        perror("listen");
        exit(1);
    }
    printf("Server able to listen on %s\n", SOCKET_PATH);

    // 5. Connection accept infinite loop
    while (1)
    {
        // If a connected socket will be there in the queue, this would create
        // a new FD entry and return its integer value
        // This is a blocking call, aka, if nothing is in queue, kernel puts the server process in S state (sleep interruptible)
        client_specific_conn_socket_fd = accept(master_socket_fd, NULL, NULL);
        if (client_specific_conn_socket_fd < 0) // this only triggers when something has really gone wrong in creating a new FD entry
        {
            perror("accept");
            continue;
        }

        printf("Client connected, we can talk to it at: %d\n", client_specific_conn_socket_fd);

        // Communication read/write loop
        while (1)
        {
            ssize_t n = read(client_specific_conn_socket_fd, buf, BUF_SIZE - 1);

            if (n <= 0) // 0 means client process stopped, crashed, or closed connection
            {
                // Client closed connection or error
                printf("Client disconnected\n");
                break;
            }

            buf[n] = '\0';

            // Remove trailing newline if present
            buf[strcspn(buf, "\n")] = '\0';

            char reply[BUF_SIZE];
            snprintf(reply, BUF_SIZE, "Hello, %s\n", buf);

            write(client_specific_conn_socket_fd, reply, strlen(reply));
        }

        // Finally when the communication is done, close the client specific socket.
        // Now client needs to make connection again.
        close(client_specific_conn_socket_fd);
    }

    // executes when the client closes the connection
    close(master_socket_fd);
    unlink(SOCKET_PATH);
    return 0;
}
