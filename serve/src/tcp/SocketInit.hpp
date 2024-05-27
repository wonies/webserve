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

class InitSocket {
 public:
  InitSocket();
  virtual ~InitSocket();
  void init(int, int &);
  void _socket(int &);
  void _bind(int, int &);
  void _listen(int &);
  void nonblock(int);
};

#endif