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

class Struct {
 public:
  int ssocket;
  struct sockaddr_in saddr;

 private:
};
