#ifndef CLIENT_H
#define CLIENT_H

#include <cstring>
#include <map>
#include <sstream>
#include <string>

#include "CGI.hpp"
#include "Server.hpp"
#include "utill.hpp"

#define SIZE_BUFF 1024
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
  int* clientptr();
  std::ostringstream oss;
  void checkError(bool);
  bool errorid();

 private:
  Server& srv;
  int _clientfd;
  bool _errorid;
};

#endif