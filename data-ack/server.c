#include <stdio.h>
#include <stdlib.h>
#include <string.h> // gives us memset()
#include <sys/socket.h>
#include <sys/un.h> // you dont write ; at the end of macro

#define SOCKET_PATH                                                            \
  "/tmp/data-ack-master-socket" // You don't write ; at the end of macro

int main() {
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

  return 0;
}
