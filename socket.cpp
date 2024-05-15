#include "socket.hpp"
#include "client.hpp"

NetworkServer::NetworkServer() {
  memset(&_socket.saddr, 0, sizeof(_socket.saddr));
  _socket.saddr.sin_family = AF_INET;
  _socket.saddr.sin_port = htons(8080);
  _socket.saddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(_socket.ssocket, (struct sockaddr *)&_socket.saddr,
           sizeof(_socket.saddr)) == -1) {
    std::cerr << "bind error" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (listen(_socket.ssocket, 10) == -1) {
    std::cerr << "listen error" << std::endl;
    exit(EXIT_FAILURE);
  }
  fcntl(_socket.ssocket, F_SETFL, O_NONBLOCK);
}

void NetworkServer::initSocket() {
  _socket.ssocket = socket(PF_INET, SOCK_STREAM, 0);
  if (_socket.ssocket == -1) {
    std::cerr << "socket error" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void NetworkServer::setupAddress() {
  memset(&_socket.saddr, 0, sizeof(_socket.saddr));
  _socket.saddr.sin_family = AF_INET;
  _socket.saddr.sin_port = htons(8080);
  _socket.saddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

void NetworkServer::bindSocket() {
  if (bind(_socket.ssocket, (struct sockaddr *)&_socket.saddr,
           sizeof(_socket.saddr)) == -1) {
    std::cerr << "bind error" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void NetworkServer::startListening() {
  if (listen(_socket.ssocket, 10) == -1) {
    std::cerr << "listen error" << std::endl;
    exit(EXIT_FAILURE);
  }
}

// void NetworkServer::evSet(int sortsocket, std::string order, std::string
// order2,
//                           std::string order3) {
//   EV_SET(&_socket.saddr, sortsocket, order, order2 | order3, 0, 0, NULL);
// }

void NetworkServer::enterKqueue() {
  if ((kq = kqueue()) == -1)
    exit_error("Falied to regisoter event with kqueue: " +
               std::string(strerror(errno)));
  EV_SET(&kev, _socket.ssocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
    exit_error("Failed tgo register event with kqueue");
    while (true) {
      nevents = kevent(kq, NULL, 0, &events[0], 10, NULL);
      if (nevents == -1)
        exit_error("kevent() error\n" + std::string(strerror(errno)));
      for (int i = 0; i < nevents; ++i) {
        struct kevent temp = events[i];
        if (temp->filter == EVFILT_READ)
        {
          if (temp->ident == ssocket)
          {
            Client eachClient;
            if ((eachClient.csocket = accept(ssocket, NULL, NULL)) == --1)
              exit_error("accept() error");
            EV_SET(&temp ,csocket, EVFILT_READ, EV_ADD  EV_ENABLE, 0, 0, NULL);
            clients[eachClient.csocket] = client; 
            fcntl(eachClient.csocket, F_SETFL, 0, O_NONBLOCK);
          }
        }
      }
    }
}

void NetworkServer::exit_error(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

NetworkServer::~NetworkServer() { close(_socket.ssocket); }
