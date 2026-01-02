#include <stdio.h>
#include <stdlib.h>
#include <string.h> // gives us memset()
#include <sys/socket.h>
#include <sys/un.h> // you dont write ; at the end of macro
#include <unistd.h>

#define SOCKET_PATH \
  "/tmp/data-ack-master-socket" // You don't write ; at the end of macro

int main()
{
  int fd_master_socket = -1;

  struct sockaddr_un socketinformation; // Need this format for bind() sys call.
  // Written & before struct because it doesn't automatically decay into a
  // pointer unlike an array type.
  memset(&socketinformation, 0,
         sizeof(socketinformation));      // clearing memory as precaution.
  socketinformation.sun_family = AF_UNIX; // unix domain socket
  // socketinformation.sun_path = "you cannot copy path like this"; // illegal
  // attempt to char [] = char *
  strncpy(socketinformation.sun_path, SOCKET_PATH,
          sizeof(socketinformation.sun_path) - 1);

  // - Kernel creates a socket object.
  // - Kernel then creates an entry in the FD table of current process.
  // - This FD Entry points to the socket object.
  fd_master_socket = socket(AF_UNIX, SOCK_STREAM, 0);

  if (fd_master_socket ==
      -1) // when socket() fails, we get this value instead of FD integer
  {
    perror("socket");
    exit(EXIT_FAILURE); // exit(1)
  }

  printf("fd of master socket: %d\n", fd_master_socket);
  printf("socket path we want %s\n", socketinformation.sun_path);

  // Before we assign a filesystem path to the master socket
  // We need to ensure a file doesn't already exist
  // with the same path
  unlink(SOCKET_PATH);

  int status_of_bind_sys_call;
  status_of_bind_sys_call =
      bind(fd_master_socket, (struct sockaddr *)&socketinformation,
           sizeof(struct sockaddr_un));

  if (status_of_bind_sys_call == -1)
  {
    printf("Unable to bind socket to pathname\n");
    perror("bind");
    return 1;
  }

  printf("master socket now associated with pathname\n");

  // Now the master socket needs to be marked as listening socket.
  // Only after master socket has been marked as passive then connection requests would come.
  // The connection queue comes into existence.
  listen(fd_master_socket, 20); // 20 is the backlog limit

  printf("Connections can be made now.\n");

  // Infinite loop of accepting connections.
  while (1)
  {
    printf("Gonna check in connection queue for a connected socket \n");
    int fd_data_socket = accept(fd_master_socket, NULL, NULL);

    if (fd_data_socket <= 0)
    {
      printf("something went wrong trying to retrieve connected socket\n");
      perror("accepting connected socket\n");
      return 1;
    }

    printf("Successfully connected with a client, fd of data socket is: %d\n", fd_data_socket);

    // Do some work here
    while (1)
    {
      // read and write system calls happen here
      char data_buffer[256];
      int conn_status;
      conn_status = read(fd_data_socket, data_buffer, sizeof(data_buffer));

      if (conn_status <= 0)
      {
        perror("read by server");
        return 1;
      }

      printf("recd this data: %s", data_buffer);
      printf("gonna send ack to the client\n");

      char msg[] = "message recd";
      int write_status;
      write_status = write(fd_data_socket, msg, sizeof(msg));

      if (write_status <= 0)
      {
        perror("server write");
        return 1;
      }
    }

    printf("server closing data socket now\n");
    close(fd_data_socket);
  };

  // Execution will never reach here
  close(fd_master_socket); // This is just a reminder of the architecture.
  return 0;
}
