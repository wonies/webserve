#include "ASocket.hpp"

ASocket::ASocket() {}
ASocket::~ASocket() {}

void ASocket::init() {
  _socket();
  _bind();
  _listen();
}

void ASocket::_socket() {
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw err_t("[SOCKET]: Socket Connect Error");
  // before setting bind. it is useful to connect port freely. [not
  // mandatory/for convient]
  int optval = 1;
  setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

// port 번호 받아오는 것 염두해두기!
void ASocket::_bind() {
  struct sockaddr_in saddr;

  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(8080);
  if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    close(sock);
    throw err_t("[SOCKET] : Bind Error");
  }
}

void ASocket::_listen() {
  if (listen(sock, 10) == -1) {
    close(sock);
    throw err_t("[SOCKET] : Listen error");
  }
  nonblock(sock);
}

void ASocket::nonblock(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
    close(fd);
    throw err_t("[FILE] : Failed to set non-blocking mode");
  }
}