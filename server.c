#include <arpa/inet.h>
#include <limits.h>
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

#define LISTEN_BACKLOG 5
#define MAX_MESSAGE_LEN 512

int start_server(char *address, int port) {
  int server_fd;
  struct sockaddr_in server_addr;
  socklen_t server_len = sizeof(server_addr);

  // Open a new ipv4 socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
    handle_error("socket");

  // Struct containing server address information
  memset(&server_addr, 0, server_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(address);

  // Need to bind socket before it can accept any connections
  if (bind(server_fd, (struct sockaddr *)&server_addr, server_len) == -1)
    handle_error("bind");

  // Need to set socket to listen
  if (listen(server_fd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  return server_fd;
}

int main(int argc, char **argv) {
  char *addr = argv[1];
  long port = strtol(argv[2], NULL, 10);

  int server_fd, client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  char message[MAX_MESSAGE_LEN];

  // If argv causes overflow we error
  if (port == LONG_MIN || port == LONG_MAX)
    handle_error("strtol");
  else if (port < 0 || port > 65535) {
    printf("Port %li not valid", port);
    exit(EXIT_FAILURE);
  }

  server_fd = start_server(addr, port);

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
