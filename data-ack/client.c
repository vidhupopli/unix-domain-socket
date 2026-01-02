#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_MASTER_SOC_PATH "/tmp/data-ack-master-socket"

int main()
{
  // Creating data socket.
  int fd_data_socket;
  fd_data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  printf("Data socket created, access using FD: %d\n", fd_data_socket);
  if (fd_data_socket < 0)
  {
    perror("socket sys call");
    return 1;
  }

  // Connecting with server's master socket
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SERVER_MASTER_SOC_PATH, sizeof(addr.sun_path));

  printf("about to attempt connection \n");
  int connection_status = connect(fd_data_socket, (struct sockaddr *)&addr,
                                  sizeof(struct sockaddr_un));

  if (connection_status == -1)
  {
    perror("client failed to connect");
    return EXIT_SUCCESS;
  }

  printf("client connected successfully\n");

  while (1)
  {
    // read and write system calls happen here.
    char data_to_send[256];
    printf("enter data to send: ");
    fgets(data_to_send, sizeof(data_to_send), stdin);

    int write_status;
    write_status = write(fd_data_socket, data_to_send, sizeof(data_to_send));
    if (write_status <= 0)
    {
      perror("client write failed");
      return 1;
    }

    char data_recd[256];
    int read_status;
    read_status = read(fd_data_socket, data_recd, sizeof(data_recd));

    if (read_status <= 0)
    {
      perror("client read");
      return 1;
    }

    printf("from server: %s\n", data_recd);
  }

  printf("Client closing data socket\n");
  close(fd_data_socket);
  return 0;
}
