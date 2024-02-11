#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define LISTEN_PORT 1337
#define LISTEN_BACKLOG 5
#define MAX_MESSAGE_LEN 512

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t server_len = sizeof(server_addr);
  socklen_t client_len = sizeof(client_addr);
  char message[MAX_MESSAGE_LEN];

  // Open a new ipv4 socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
    handle_error("socket");

  // Struct containing server address information
  memset(&server_addr, 0, server_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LISTEN_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  // Need to bind socket before it can accept any connections
  if (bind(server_fd, (struct sockaddr *)&server_addr, server_len) == -1)
    handle_error("bind");

  // Need to listen on socket
  if (listen(server_fd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  // Wait for a connection and open socketfd to client
  client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd == -1)
    handle_error("accept");

  // TODO: Poll for messages and responses rather than single
  // Receive message from client socket
  memset(&message, 0, MAX_MESSAGE_LEN);
  if (recv(client_fd, &message, MAX_MESSAGE_LEN, 0) == -1)
    handle_error("recv");
  printf("[recv]: %s", message);

  // Send response to client from server
  memset(&message, 0, MAX_MESSAGE_LEN);
  if (read(STDIN_FILENO, &message, MAX_MESSAGE_LEN) == -1)
    handle_error("read stdin");
  if (send(client_fd, message, MAX_MESSAGE_LEN, 0) == -1)
    handle_error("send");

  // Clean up open sockets
  if (close(client_fd) == -1)
    handle_error("close client");
  if (close(server_fd) == -1)
    handle_error("close server");

  return EXIT_SUCCESS;
}
