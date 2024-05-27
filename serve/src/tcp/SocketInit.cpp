#include "SocketInit.hpp"

InitSocket::InitSocket() {}
InitSocket::~InitSocket() {}

void InitSocket::init(int port, int &sock) {
  _socket(sock);
  _bind(port, sock);
  _listen(sock);
  std::clog << "[Connect Port] : " << port << std::endl;
}

void InitSocket::_socket(int &sock) {
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw err_t("[SOCKET]: Socket Connect Error");
  int optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void InitSocket::_bind(int port, int &sock) {
  struct sockaddr_in saddr;

  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);
  if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    close(sock);
    throw err_t("[SOCKET] : Bind Error");
  }
}

void InitSocket::_listen(int &sock) {
  if (listen(sock, 100) == -1) {
    close(sock);
    throw err_t("[SOCKET] : Listen error");
  }
  nonblock(sock);
}

void InitSocket::nonblock(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
    close(fd);
    throw err_t("[FILE] : Failed to set non-blocking mode");
  }
}