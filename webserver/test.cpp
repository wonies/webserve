#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket error");
    return 1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("connect error");
    close(sock);
    return 1;
  }

  const char *message = "Hello, server!";
  if (send(sock, message, strlen(message), 0) == -1) {
    perror("send error");
    close(sock);
    return 1;
  }

  char buffer[1024];
  ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
  if (bytes_received == -1) {
    perror("recv error");
  } else {
    buffer[bytes_received] = '\0';
    std::cout << "Received from server: " << buffer << std::endl;
  }
  close(sock);
  return 0;
}