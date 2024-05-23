#include "client.hpp"
#include "socket.hpp"
#include "struct.hpp"

void exit_error(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

int main() { NetworkServer server; }