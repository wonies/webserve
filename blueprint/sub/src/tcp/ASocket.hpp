#ifndef SOCKET_INIT_HPP
#define SOCKET_INIT_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "error.hpp"
#include "log.hpp"
#include "structure.hpp"

#define LOOP 1

class ASocket {
 public:
  int sock;
  //   std::vector<int> socklist; //나중에 socket list넣을때
  ASocket();
  virtual ~ASocket();
  void init();
  void _socket();
  void _bind();
  void _listen();
  void nonblock(int);
};

#endif