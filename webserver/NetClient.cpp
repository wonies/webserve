#include "NetClient.hpp"

NetClient::NetClient() : _clientfd(-1) { return; }

NetClient::NetClient(int fd) : _clientfd(fd) {}

NetClient::~NetClient() { close(_clientfd); }

NetClient::NetClient(const NetClient& other) : _clientfd(other._clientfd) {
  oss << other.oss.str();
}

NetClient& NetClient::operator=(const NetClient& other) {
  if (this != &other) {
    _clientfd = other._clientfd;
    oss.str("");
    oss << other.oss.str();
  }
  return *this;
}

int NetClient::getClientFd() const { return _clientfd; }