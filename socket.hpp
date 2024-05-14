#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "client.hpp"

class NetworkServer {
 public:
  NetworkServer();
  ~NetworkServer();
  void initSocket();
  void setupAddress();
  void bindSocket();
  void startListening();
  void enterKqueue();
  void exit_error(const std::string &msg);
  void NetworkServer::evSet(int socket, std::string order, std::string order2,
                            std::string order3);

 private:
  struct Socket {
    int ssocket;
    sockaddr_in saddr;
  } _socket;
  int kq;
  std::map<int, Client> clients;
  struct kevent kev;  // 등록할 애들 -> 지역변수로하면됨
  int nevents;

 public:
  std::vector<struct kevent> events;  // 폴링 : 발생한 이벤트에 대한 목록
  // struct kevent events[10];
};

#endif