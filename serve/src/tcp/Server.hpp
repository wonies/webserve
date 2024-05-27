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

  // preset kqueue & server
  void preset();
  void _kqueue();
  void _server(int);
  // utils
  void disconnect(int);
  void setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void *udata);

  // event run
  void run(vec_config_t &);

  // run inter
  int _cnttake();
  void _read(struct kevent &);
  void _write(struct kevent &);
  void _proc(struct kevent &);
  void _timer(struct kevent &);
  bool eventerr(struct kevent &);
  void confset(vec_config_t &);

  //_read
  void connectEvent(int);
  void clientEvent(struct kevent &);
  void addClient(int);

  bool timing;

 private:
  int kq;
  int nevents;
  struct timespec _time;
  std::vector<struct kevent> events;
  std::map<int, Client *> clients;
  std::map<int, std::string> _type;
  std::vector<int> _portarray;
  const vec_config_t &_confs;
};

#endif