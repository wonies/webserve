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
  bool eventerr(struct kevent &);
  void confset(vec_config_t &);

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

  // socket_server -> 여러개/ client ->
  // fd 4/ 3e97ewq8e78 / cgi / ----
  // fd : "serv" / "cl" / "cgi fd"
  // else -> cgi -> 어떤 소켓이랑 연결이 되었는 지 알 수 없음
  // udata에 저장 => fd값은 재할당이 될 수 있음
  // udata -> (void *) // client server 문자열을 저장해놓고
  // cgi <-> sock // cgi udata에 소켓번호저장.

  const vec_config_t &_confs;
};

#endif