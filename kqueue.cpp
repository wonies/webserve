#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <vector>

class Cmanager {
 public:
  Cmanager(int socket_fd) : client_fd(socket_fd), data("") {
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
  }
  ~Cmanager() { close(client_fd); }
  bool receiveData() {
    char buffer[1024];
    int receiver = recv(client_fd, buffer, 1024);
    if (receiver > 0) {
      buffer[receiver] = '\0';
      oss << buffer;
      return true;
    } else if (receiver < 0)
      std::cerr << "Error reading from client!" << std::endl;
    return false;
  }
  bool writeData(const string &msg) {
    int sent = send(client_fd, msg.c_str(), msg.size(), 0);
    if (sent < 0) {
      std::cerr << "Error writing to client : " << strerror(errno) << std::endl;
      return false;
    }
    return true;
  }
  std::string getData() const {
    return oss.str();
  }  // -> 동현이오빠코드 가공한 데이터 => html응답데이터?
  // bool writeData()
 private:
  int client_fd;
  std::ostringstream oss;
};

void exit_error(const string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

int main() {
  int ssocket;
  struct sockaddr_in saddr;
  if ((ssocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    exit_error("socket error " + string(strerror(errno)));
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(8080);
  if (bind(ssocket, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    exit_error("bind error " + string(strerror(errno)));
  if (listen(ssocket, 5) == -1)
    exit_error("liste error" + string strerror(errno));
  fcntl(ssocket, F_SETFL, O_NONBLOCK);  // non-blocking설정 / 서버가 여러개 일
                                        // 수 있음 (config 등록 시 마다 다름_!)
  int kq;
  if ((kq = kqueue()) == -1)
    exit_error("kqueue error" + string(strerror(errno)));
  std::map<int, Cmanager> clients;  // buffer request => osstream
  struct kevent kev;
  EV_SET(&saddr, ssocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
         NULL);  // event 등록
  if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
    exit_error("Failed to register event with kqueue: ") +
        string(strerror(errno));
  int nevents;
  std::vector<struct kevent> event[10];  // struct의 배열화
  while (1) {
    nevents = kevent(kq, NULL, 0, &event, 10, NULL);
    if (nevents == -1) exit_error("kevent()error\n" + string(strerror(errno)));
    for (int i = 0; i < nevents; ++i) {
      struct kevent temp = event[i];
      if (temp->filter == EVFILT_READ) {
        if (temp->ident == ssocket)  // server 일 d때
        {
          int csocket;
          if ((csocket = accept(ssocket, NULL, NULL)) == -1)
            exit_error("accept( ) error\n" + string(strerror(errno)));
          EV_SET(&saddr, csocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
                 NULL);  // event 등록
          Cmanager client;
          clients[csocket] = client;
          fcntl(csocket, F_SETFL, O_NONBLOCK);
        } else  // client일 때
        {
          if (client.receiveData() == 0) {
            exit_error("client receive error");
          }
          // 응답코드
          EV_SET(&saddr, ssocket, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0,
                 NULL);  // event 등록
        }
      } else if (temp->filter == EVFILT_WRITE) {
        if (client.writeData(client.getData()) == 0)
          exit_error("client write error");
      }
    }
  }
}

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>

class NetworkServer {
 public:
  NetworkServer() {
    st.ssocket = socket(PF_INET, SOCK_STREAM, 0);
    if (st.ssocket == -1) {
      std::cerr << "socket error" << std::endl;
      exit(EXIT_FAILURE);
    }

    memset(&st.saddr, 0, sizeof(st.saddr));
    st.saddr.sin_family = AF_INET;
    st.saddr.sin_port = htons(8080);
    st.saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(st.ssocket, (struct sockaddr *)&st.saddr, sizeof(st.saddr)) ==
        -1) {
      std::cerr << "bind error" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (listen(st.ssocket, 10) == -1) {
      std::cerr << "listen error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  ~NetworkServer() { close(st.ssocket); }

 private:
  struct Socket {
    int ssocket;
    sockaddr_in saddr;
  } st;
};

int main() {
  NetworkServer server;
  return 0;
}

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

class NetworkServer {
 public:
  NetworkServer() {
    initializeSocket();
    setupAddress();
    bindSocket();
    startListening();
  }

  ~NetworkServer() {
    if (_socket.ssocket != -1) {
      close(_socket.ssocket);
    }
  }

 private:
  struct Socket {
    int ssocket;
    sockaddr_in saddr;
  } _socket;

  void initializeSocket() {
    _socket.ssocket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket.ssocket == -1) {
      std::cerr << "socket error" << std::endl;
      exit(EXIT_FAILURE);
    }
    fcntl(_socket.ssocket, F_SETFL, O_NONBLOCK);
  }

  void setupAddress() {
    memset(&_socket.saddr, 0, sizeof(_socket.saddr));
    _socket.saddr.sin_family = AF_INET;
    _socket.saddr.sin_port = htons(8080);
    _socket.saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  }

  void bindSocket() {
    if (bind(_socket.ssocket, (struct sockaddr *)&_socket.saddr,
             sizeof(_socket.saddr)) == -1) {
      std::cerr << "bind error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void startListening() {
    if (listen(_socket.ssocket, 10) == -1) {
      std::cerr << "listen error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
};

int main() {
  NetworkServer server;  // 서버 인스턴스 생성
  return 0;
}