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

  void enterKqueue();
  void exit_error(const std::string &);
  void createKqueue();
  void setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void *udata);

  void manageEvent();
  void serverEvent();
  void addClient(int);
  // void readClient(int fd);
  void writeClient(int);
  int takeEvents();
  void _read(struct kevent &);
  void readConnect();
  void readClient(struct kevent &);
  void readCgi(struct kevent &);

  void disconnect(int);
  const vec_config_t &config(void) const { return _confs; }
  const config_t &configDefault(void) const { return _confs.at(0); }
  void run();
  void preset();
  void _kqueue();
  void _server();
  int takeEvent();
  void _write(struct kevent &);
  void connectEvent();
  void clientEvent(struct kevent &);
  void _write(int);

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