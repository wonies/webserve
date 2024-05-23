#ifndef CLIENT_H
#define CLIENT_H

#include <cstring>
#include <map>
#include <sstream>
#include <string>

#include "Server.hpp"
#include "utill.hpp"

#define SIZE_BUF 1024
#define SIZE_CRLF 2
#define SIZE_CHUNK_HEAD 3

#include "Transaction.hpp"

class Server;

class Client {
 public:
  Client(int fd, Server&);
  ~Client();
  int getfd() const;
  void request();
  bool respond();
  
  msg_buffer_t in;
  msg_buffer_t out;

  Transaction* action;
  process_t subprocs;
  const msg_buffer_t& buffer() const;
  msg_buffer_t& get_in();
  const Server& server() const;
  // process_t& get_process();

  std::ostringstream oss;
  char buf[SIZE_BUF];
 private:
  Server& srv;
  int _clientfd;
};

#endif