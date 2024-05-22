#include "Server.hpp"

Server::Server(const vec_config_t &confs) : events(10), _confs(confs) {
  preset();
}

Server::~Server() {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }
  close(sock);
  close(kq);
}

void Server::preset() {
  _time.tv_sec = 100;
  _kqueue();
  _server();
}

void Server::_kqueue() {
  if ((kq = kqueue()) == -1) throw err_t(" [EVENT] : Failed to create kqueue");
}

void Server::_server() {
  init();
  setEvent(sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  _type[sock] = "serv";  // socket도 배열화 해야하는 건 아닌가?
}

void Server::setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void *udata) {
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
    throw err_t("[EVENT] : Failed to register events");
  }
}

void Server::disconnect(int) {
  std::map<int, Client *>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    close(fd);
    delete it->second;
    clients.erase(it);
  }
}

void Server::addClient(int fd) {
  Client *newClient = new Client(fd, *this);
  clients[fd] = newClient;
  _type[fd] = "cl";
  nonblock(fd);
}

void Server::connectEvent() {
  int fd;
  if ((fd = accept(sock, NULL, NULL)) == -1)
    throw err_t("Failed to accept socket fd");
  std::clog << "Accept by Server" << std::endl;
  addClient(fd);
  setEvent(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void Server::clientEvent(struct kevent &event) {
  std::map<int, Client *>::iterator it = clients.find(event.ident);
  if (it != clients.end()) {
    Client *tmp = it->second;
    std::clog << "Received request from client : " << event.ident << std::endl;
    tmp->request();
  } else
    std::clog << "Client not FOUND : " << event.ident << std::endl;
}

void Server::_read(struct kevent &event) {
  int id = event.ident;
  if (_type[id] == "serv")
    connectEvent();
  else if (_type[id] == "cl")
    clientEvent(event);
}

void Server::_write(struct kevent &event) {
  int fds = event.ident;
  std::clog << "[WRITE] Check if it's in   " << fds << std::endl;
  std::map<int, Client *>::iterator it = clients.find(fds);
  if (it != clients.end()) {
    Client *client = it->second;
    client->sendData();
    if (!(it->second->sendData())) {
      disconnect(fds);
      std::clog << "_write disconnect" << std::endl;
    }
  }
}

int Server::takeEvent() {
  int nevent;
  nevent = kevent(kq, NULL, 0, &events[0], 10, NULL);
  if (nevent == -1) throw("Failed to take place a event");
  return nevent;
}

void Server::run() {
  while (true) {
    nevents = takeEvent();
    for (int i = 0; i < nevents; ++i) {
      struct kevent &temp = events[i];
      if (temp.filter == EVFILT_READ)
        _read(temp);
      else if (temp.filter == EVFILT_WRITE)
        _write(temp);
    }
  }
}