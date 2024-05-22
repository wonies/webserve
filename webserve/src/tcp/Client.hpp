#ifndef CLIENT_H
#define CLIENT_H

#include <cstring>
#include <map>
#include <sstream>
#include <string>

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
  int getFd() const;
  void request();
  std::ostringstream oss;

  char buf[SIZE_BUFF];
  // ssize_t         byte;

  bool sendData();

  msg_buffer_t in;
  msg_buffer_t out;

  Transaction* action;
  process_t subprocs;

  const msg_buffer_t& buffer() const;
  const Server& server() const;
  msg_buffer_t& get_in();
  // process_t& get_process();

 private:
  Server& srv;
  int _clientfd;
};

#endif

// client  server 넣어주기