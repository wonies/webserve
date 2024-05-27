#include "Server.hpp"

Server::Server(const vec_config_t &confs)
    : timing(FALSE), events(10), _confs(confs) {
  preset();
}

Server::~Server() {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }
}

void Server::preset() {
  _time.tv_sec =
      100;  // 이벤트 간의 간격 다음 이벤트는 100초안에는 무조건 발생시킨다.
  _kqueue();
}

void Server::_kqueue() {
  if ((kq = kqueue()) == -1) throw err_t(" [EVENT] : Failed to create kqueue");
}

void Server::_server(int port) {
  int sock = 0;
  init(port, sock);
  setEvent(sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  _type[sock] = "serv";
}

void Server::setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void *udata) {
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
    std::cerr << "Failed to register events: " << strerror(errno) << std::endl;
}

void Server::disconnect(int clfd) {
  std::map<int, Client *>::iterator it = clients.find(clfd);
  if (it != clients.end()) {
    close(clfd);
    delete it->second;
    clients.erase(it);
    _type.erase(clfd);
  }
}

void Server::addClient(int fd) {
  Client *newClient = new Client(fd, *this);
  clients[fd] = newClient;
  _type[fd] = "cl";
  nonblock(fd);
}

void Server::connectEvent(int id) {
  int fd;
  if ((fd = accept(id, NULL, NULL)) == -1)
    throw err_t("Failed to accept socket fd");
  addClient(fd);
  setEvent(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  setEvent(fd, EVFILT_TIMER, EV_ADD, 0, 30000, NULL);
  timing = TRUE;
}

void Server::clientEvent(struct kevent &event) {
  std::map<int, Client *>::iterator it = clients.find(event.ident);
  if (it != clients.end()) {
    Client *tmp = it->second;
    tmp->request();
  }
}

bool Server::eventerr(struct kevent &event) {
  if (event.flags & EV_ERROR) {
    disconnect(event.ident);
    return true;
  }
  if ((event.flags & EV_EOF) && (event.filter != EVFILT_PROC)) {
    std::map<int, Client *>::iterator it = clients.find(event.ident);
    if (it != clients.end()) {
      disconnect(event.ident);
      return true;
    }
  }
  return false;
}

int Server::_cnttake() {
  int nevent;
  nevent = kevent(kq, NULL, 0, &events[0], 10, NULL);
  if (nevent == -1) throw("Failed to take place a event");
  return nevent;
}

void Server::_read(struct kevent &event) {
  int id = event.ident;
  if (_type[id] == "serv")
    connectEvent(id);
  else if (_type[id] == "cl")
    clientEvent(event);
}

void Server::_write(struct kevent &event) {
  int fds = event.ident;
  std::map<int, Client *>::iterator it = clients.find(fds);
  Client &_client = *it->second;
  if (it != clients.end()) {
    if (!(_client.respond())) {
      disconnect(fds);
      return;
    }
  }
  if (_client.subprocs.pid && _client.errorid()) {
    _client.checkError(FALSE);
    return;
  }
  if ((!(_client.subprocs.pid)) && _client.errorid()) {
    disconnect(fds);
    _client.checkError(FALSE);
    return;
  }
}

void Server::_proc(struct kevent &event) {
  int client;
  if (event.udata != NULL) client = *(static_cast<int *>(event.udata));

  std::map<int, Client *>::iterator it = clients.find(client);
  Client &cl = *it->second;
  try {
    // CGI::wait(cl.subprocs);
    if (WEXITSTATUS(cl.subprocs.stat) != EXIT_SUCCESS) {
      throw errstat_t(500, "the CGI failed to exit as SUCCESS");
    }
    CGI::readFrom(cl.subprocs, cl.out.body);
    CGI::build(cl.out);
    cl.in.reset();
    cl.subprocs.reset();
    setEvent(event.ident, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    setEvent(client, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
  } catch (const errstat_t &e) {
    log("TCP\t: " + str_t(e.what()));
    cl.out.reset();
    close(cl.subprocs.fd[R]);
    Transaction::buildError(e.code, cl);
    cl.checkError(TRUE);
    setEvent(event.ident, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    setEvent(client, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
  }
}

void Server::_timer(struct kevent &event) {
  int udata = 0;
  int fd = event.ident;
  if (event.udata != NULL) udata = *(static_cast<int *>(event.udata));
  std::map<int, Client *>::iterator it = clients.find(fd);
  it = clients.find(fd);
  try {
    if (it != clients.end()) {
      throw errstat_t(503, "Time out request on HTTP");
    } else if (event.udata != NULL) {
      throw errstat_t(503, "Time out request IN CGI");
    }
  } catch (const errstat_t &e) {
    log("TCP\t: " + str_t(e.what()));
    if (it != clients.end()) {
      Client &cl = *it->second;
      cl.checkError(TRUE);
      cl.out.reset();
      Transaction::buildError(e.code, cl);
      setEvent(event.ident, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
      return;
    } else if (event.udata != NULL) {
      it = clients.find(udata);
      if (it != clients.end() && it->second != NULL) {
        Client &cl = *it->second;
        cl.checkError(TRUE);
        if (cl.subprocs.pid) {
          cl.out.reset();
          cl.checkError(TRUE);
          Transaction::buildError(e.code, cl);
          setEvent(udata, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
        }
      }
    }
  }
}

void Server::confset(vec_config_t &conf) {
  vec_config_t::iterator it = conf.begin();
  vec_config_t::iterator endit = conf.end();
  for (; it != endit; ++it) {
    if (distance(_portarray, it->listen) == NOT_FOUND) _server(it->listen);
    _portarray.push_back(it->listen);
  }
}

void Server::run(vec_config_t &conf) {
  confset(conf);
  while (true) {
    nevents = _cnttake();
    try {
      for (int i = 0; i < nevents; ++i) {
        struct kevent &temp = events[i];
        if (eventerr(temp)) continue;

        if (temp.filter == EVFILT_READ)
          _read(temp);
        else if (temp.filter == EVFILT_WRITE)
          _write(temp);
        else if (temp.filter == EVFILT_PROC)
          _proc(temp);
        else if (temp.filter == EVFILT_TIMER)
          _timer(temp);
      }

    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
    }
  }
}
