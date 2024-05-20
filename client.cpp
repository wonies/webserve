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


 // 기존 클라이언트 소켓에서 데이터 읽기
                    char buffer[1024];
                    ssize_t bytesRead = recv(temp.ident, buffer, sizeof(buffer) - 1, 0);
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        std::cout << "Read " << bytesRead << " bytes from client socket " << temp.ident << ": " << buffer << std::endl;
                    } else if (bytesRead == 0) {
                        // 클라이언트 소켓 정상 종료
                        std::cout << "Client socket " << temp.ident << " closed" << std::endl;
                        close(temp.ident);
                        clients.erase(temp.ident);