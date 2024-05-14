#include "client.hpp"

Client::Client(void) {}

Client::Client(int socket_fd) : client_fd(socket_fd) {
  fcntl(client_fd, F_SETFL, O_NONBLOCK);
}

Client::~Client() { close(client_fd); }

bool Client::receiveData() {
  char buf[1024];
  int rec = recv(client_fd, buf, 1024, 0);
  if (rec > 0) {
    buf[rec] = '\0';
    oss << buf;
    return true;
  } else if (rec < 0)
    std::cerr << "Error reading from client!" << std::endl;
  return false;
}

bool Client::writeData(const std::string &msg) {
  int sent = send(client_fd, msg.c_str(), msg.size(), 0);
  if (sent < 0) {
    std::cerr << "Error writing to client : " << strerror(errno) << std::endl;
    return false;
  }
  return true;
}