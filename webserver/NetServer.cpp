#include "NetServer.hpp"

NetServer::NetServer() {
  initSocket();
  setupBind();
  startListening();
  createKqueue();
  serverEvent();
  manageEvent();
}

NetServer::~NetServer() { close(sock); }

void NetServer::initSocket() {
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    exit_error("socket error");
}

void NetServer::setupBind() {
  struct sockaddr_in saddr;

  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(8080);
  if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    exit_error("bind error");
  fcntl(sock, F_SETFL, O_NONBLOCK);  // non-blocking
}

void NetServer::startListening() {
  if (listen(sock, 10) == -1) exit_error("listen error");
}

void NetServer::exit_error(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

void NetServer::createKqueue() {
  if ((kq = kqueue()) == -1) exit_error("kqueue error");
}

void NetServer::serverEvent() {
  enrollEvent(sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void NetServer::enrollEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                            uint32_t fflags, intptr_t data, void *udata) {
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
    perror("Failed to register event with kqueue");
    exit_error("Failed to register event with kqueue");
  }
}

void NetServer::addClient(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);  // socket의 기본 모드는 블로킹이기 떄문에
                                   // nonblocking설정을 소켓생성시에 해주면 됨
  NetClient newClient(fd);
  clients[fd] = newClient;
}

void NetServer::readClient(int fd) {
  std::map<int, NetClient>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    NetClient &client = it->second;
    char buf[1024];
    ssize_t readbyte = recv(fd, buf, 1024, 0);
    if (readbyte > 0) {
      buf[readbyte] = '\0';
      client.oss << buf;
      enrollEvent(fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);
    } else if (readbyte <= 0) {
      if (readbyte < 0) perror("recv error\n");
      close(fd);
      clients.erase(fd);
    }
  }
}

void NetServer::writeClient(int fd) {
  std::map<int, NetClient>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    NetClient &client = it->second;
    std::string data = client.oss.str();
    ssize_t sentBytes = send(fd, data.c_str(), data.size(), 0);
    enrollEvent(fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);

    if (sentBytes == -1) {
      perror("send error\n");
      close(fd);
      clients.erase(fd);
    } else {
      // Clear the buffer after sending data
      client.oss.str("");
      client.oss.clear();
    }
  }
}

void NetServer::manageEvent() {
  while (1) {
    nevents = kevent(kq, NULL, 0, events, 10, NULL);
    if (nevents == -1) exit_error("event manage Error");
    for (int i = 0; i < nevents; ++i) {
      struct kevent temp = events[i];
      if (temp.filter == EVFILT_READ) {
        if (temp.ident == sock) {
          if ((fd = accept(sock, NULL, NULL)) == -1)
            exit_error("client create error");
          addClient(fd);
          enrollEvent(fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);
        } else {
          readClient(temp.ident);
        }
      } else if (temp.filter == EVFILT_WRITE)
        writeClient(temp.ident);
    }
  }
}
