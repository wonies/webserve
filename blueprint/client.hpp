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

class Client {
 public:
  Client();
  Client(int socket_fd);
  ~Client();
  bool receiveData(void);
  bool writeData(const std::string &msg);

 private:
  int client_fd;
  std::ostringstream oss;
};