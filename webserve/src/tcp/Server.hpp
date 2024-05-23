#ifndef SERVER_HPP
#define SERVER_HPP

#include "ASocket.hpp"
#include "Client.hpp"
#include "structure.hpp"

class Client;

class Server : public ASocket {
 public:
  Server(const vec_config_t &);
  ~Server();

  const vec_config_t &config(void) const { return _confs; }
  const config_t &configDefault(void) const { return _confs.at(0); }

  //preset kqueue & server
  void preset();
  void _kqueue();
  void _server();
  //utils
  void disconnect(int);
  void setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void *udata);
  
  //event run
  void run();

  //run inter
  int _cnttake();
  void _read(struct kevent &);
  void _write(struct kevent &);
  

  //_read
  void connectEvent();
  void clientEvent(struct kevent &);
  void addClient(int);

 private:
  int kq;
  int nevents;
  int fd;
  struct timespec _time;
  std::vector<struct kevent> events;
  std::map<int, Client *> clients;
  std::map<int, std::string> _type;

  const vec_config_t &_confs;
};

#endif