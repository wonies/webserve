#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class NetClient {
 public:
  NetClient();
  NetClient(int fd);
  ~NetClient();
  NetClient(const NetClient& other);  // 복사 생성자 -> client class화를 위해
  NetClient& operator=(const NetClient& other);  // 복사 할당 연산자
  int getClientFd() const;
  bool receiving();
  std::ostringstream oss;

 private:
  int _clientfd;
};

#endif