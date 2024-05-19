#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "filter.hpp"
#include "structure.hpp"

std::vector<std::string> split(const std::string &s, char deli);

config_s::config_s() : client_max_body(0) {}

location_s::location_s(const config_s &config) : index_auto(false) {}

struct isNotSpace {
  bool operator()(unsigned char ch) const { return !std::isspace(ch); }
};

std::string toUpper(const std::string &str) {
  std::string res = str;
  for (size_t i = 0; i < res.size(); ++i) {
    if (res[i] >= 'a' && res[i] <= 'z')
      res[i] -= 32;
    else if (!std::isalpha(res[i]) && res[i] != ' ')
      throw std::runtime_error("Invalid alphabet");
  }
  return res;
}

std::string readfile(const std::string &filepath) {
  std::ifstream file("default.config");
  if (!file.is_open()) throw std::runtime_error("Failed to open file: ");
  file.seekg(0, std::ios::end);
  std::size_t length = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string content;
  content.resize(length);
  file.read(&content[0], length);
  file.close();
  return content;
}

std::vector<std::string> split(const std::string &s, char deli) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenstream(s);
  while (std::getline(tokenstream, token, deli)) {
    trim(token);
    if (!token.empty()) tokens.push_back(token);
  }
  return tokens;
}

void trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), isNotSpace()));
  s.erase(std::find_if(s.rbegin(), s.rend(), isNotSpace()).base(), s.end());
}

void _print(config_t config) {
  std::cout << "server block end  : " << std::endl;
  // std::cout << "config names : " << config.names[0] << std::endl;
  for (size_t i = 0; i < config.names.size(); ++i)
    std::cout << "server name : " << config.names[i] << std::endl;
  std::cout << "listen port : " << config.listen << std::endl;
  std::cout << "client max body size : " << config.client_max_body << std::endl;
  for (size_t i = 0; i < config.locations.size(); ++i) {
    const location_t &loc = config.locations[i];
    std::cout << "location : " << i << ": " << std::endl;
    std::cout << "alias : " << loc.alias << std::endl;
    std::cout << "root : " << loc.root << std::endl;
    std::cout << "index auto : " << loc.index_auto << std::endl;
    std::cout << "allow : ";
    for (size_t j = 0; j < loc.allow.size(); ++j)
      std::cout << loc.allow[j] << " ";
    std::cout << std::endl;
  }
}
config_t parse(const std::string &content) {
  std::istringstream stream(content);
  std::ostringstream oss;
  std::string line;
  config_t config;
  location_t currloc(config);
  bool inlocation = false;
  bool serverblock = false;
  bool locblock = false;

  while (std::getline(stream, line)) {
    trim(line);
    if (line.empty()) continue;
    if (line == "server {") {
      if (serverblock)
        throw std::runtime_error("Nested server blocks are not allowed");
      config = config_t();
      serverblock = true;
      oss.str("");
    } else if (line == "}") {
      if (locblock) {
        config.locations.push_back(currloc);
        locblock = false;
      }
      if (serverblock) {
        _print(config);
      }
      serverblock = false;
    }
  }
  else if (serverblock) {
    oss << line << "\n";
    if (line.find("listen") != std::string::npos) {
      std::vector<std::string> tokens = split(line, ' ');
    }
  }
}

int main() {
  try {
    std::string content = readfile("default.config");
    config_t config = parse(content);
  } catch (const std::exception &ex) {
    std::cerr << "Error : " << ex.what() << std::endl;
  }
  return 0;
}
