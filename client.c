#include <arpa/inet.h>
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
  int server_fd;
  struct sockaddr_in server_addr;
  socklen_t server_len = sizeof(server_addr);
  char message[MAX_MESSAGE_LEN];

  // Open a new ipv4 socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
    handle_error("socket");

  // Struct containing server address information
  memset(&server_addr, 0, server_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LISTEN_PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect socket to server
  if (connect(server_fd, (struct sockaddr *)&server_addr, server_len) == -1)
    handle_error("connect");

  // TODO: Poll for messages and responses rather than single
  // Send message to server from client
  memset(&message, 0, MAX_MESSAGE_LEN);
  if (read(STDIN_FILENO, &message, MAX_MESSAGE_LEN) == -1)
    handle_error("read stdin");
  if (send(server_fd, &message, MAX_MESSAGE_LEN, 0) == -1)
    handle_error("send");

  // Receive message from server socket
  memset(&message, 0, MAX_MESSAGE_LEN);
  if (recv(server_fd, message, MAX_MESSAGE_LEN, 0) == -1)
    handle_error("send");
  printf("[recv]: %s", message);

  // Clean up open sockets
  if (close(server_fd) == -1)
    handle_error("close server");

  return EXIT_SUCCESS;
}
