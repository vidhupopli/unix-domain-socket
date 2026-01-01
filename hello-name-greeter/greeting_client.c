#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_PATH "/tmp/greeting.sock"
#define BUF_SIZE 256

int main()
{
    int socket_fd; // there is going to be only one socket on the client side
    struct sockaddr_un addr;
    char buf[BUF_SIZE];

    // 1. Create socket: only sets up socket obj, and creates socket_fd table entry
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("socket");
        exit(1);
    }
    printf("client socket fd: %d\n", socket_fd);

    // 2. Prepare address
    memset(&addr, 0, sizeof(addr));                                 // initting and cleaning up
    addr.sun_family = AF_UNIX;                                      // we want unix domain socket
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); // setting path of socket we wanna communicate at

    // 3. Connect
    // This creates a new socket on the server side called connection socket
    // our socket is now a pair of this
    // this new socket is now placed in the queue that exists because of server side listen() call
    if (connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    // From this point onwards we can send actual data
    printf("Connected to server\n");

    // 4. Communication loop
    while (1)
    {
        printf("About to ask for name\n");
        printf("Enter name: ");
        fflush(stdout); // push anything in the output buffer to the terminal

        // Take user input from console. If nothing then close process.
        if (!fgets(buf, BUF_SIZE, stdin))
        {
            break;
        }

        write(socket_fd, buf, strlen(buf));             // writing  to the socket
        ssize_t n = read(socket_fd, buf, BUF_SIZE - 1); // waiting for socket to reply. There is a read queue attached to the socket. It was created at the time of client doing connnect()

        // server closed the connection, doesn't reply
        if (n <= 0) // 0 means server process stopped, crashed, or closed the connection
        {
            printf("Server closed connection\n");
            break;
        }

        buf[n] = '\0';                  // last byte we want to be null terminated.
        printf("Server says: %s", buf); // printing what the sock sent back
    }

    // executes when the user doesn't give any input
    // executes also when server doesn't respond but rather closes the connection
    close(socket_fd);
    return 0;
}
