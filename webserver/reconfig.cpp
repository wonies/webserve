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

bool isNumber(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), isNotSpace()));
  s.erase(std::find_if(s.rbegin(), s.rend(), isNotSpace()).base(), s.end());
}

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


std::string readfile(const std::string &filepath) {
  std::ifstream file("default.config");
  if (!file.is_open()) throw std::runtime_error("Failed to open file: ");
  file.seekg(0, std::ios::end);
  std::size_t length = file.tellg();
  file.seekg(0, std::ios::beg);

  // 파일의 내용을 문자열로 읽습니다.
  std::string content;
  content.resize(length);
  file.read(&content[0], length);
  file.close();
  // std::cout << content << std::endl;
  // std::cout << "config setting\n";
  return content;
}


config_t parse(const std::string &content)
{
    std::istringstream stream(content);
    std::ostringstream oss;
    std::string line;
    config_t config;
    location_t currloc(config);
    bool inloc = false;
    bool serverblock = false;
    bool locblock = false;

    while (std::getline(stream, line))
    {
        trim(line);
        if (line.empty()) continue;
        if (line == "server {")
        {
            if (serverblock)
                throw std::runtime_error("Nested server blocks are not allowed");
            config = config_t();
            serverblock = true;
            oss.str("");
        }
        else if (line == "}")
        {
            if (locblock)
            {
                config.locations.push_back(currloc);
                locblock = false;
            }
            else if (serverblock)
            {
                std::cout << "server block end  : " << std::endl;
                for (size_t i = 0; i < config.names.size(); ++i)
                    std::cout << "server name : " << config.names[i] << std::endl;
                std::cout << "listen port : " << config.listen << std::endl;
                std::cout << "client max body size : " << config.client_max_body
                  << std::endl;
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
                for (size_t k = 0; k < loc.index.size(); ++k)
                    std::cout << "index : " << loc.index[k] << std::endl;
                std::cout << std::endl;

                }
                serverblock = false;
            }
        }
        else if (serverblock)
        {
            oss << line << "\n";
            if (line.find("listen") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "listen")
                {
                    std::string portStr = tokens[1];
                    if (isNumber(portStr)) 
                    {
                        if (config.listen.size() > 1)
                            std::cout << "----listen----\n";
                        config.listen = std::atoi(portStr.c_str());
                        std::cout << "tokens : " << portStr;
                        std::cout << std::endl;
                    }
                }
                else
                    throw std::runtime_error("Invalid 'listen' directive");
            }
            else if (line.find("server_name") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "server_name")
                {
                    for (size_t i = 1; i < tokens.size(); ++i)
                    {
                        std::string servername = tokens[i];
                        config.names.push_back(servername);
                    }
                    for (size_t i = 0; i < config.names.size(); ++i)
                        std::cout << config.names[i] << " ";
                    std::cout << std::endl;
                }
                else
                    throw std::runtime_error("Invalid 'server_name' directive");
            }
            else if (line.find("Client_body_size") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "Client_body_size")
                {
                    std::string str = tokens[1];
                    size_t ssize = std::atoi(str.substr(0, str.size()-1).c_str());
                    config.client_max_body = ssize; 
                }
                else
                    throw std::runtime_error("Invalid 'Client_body_size' directive");
            }
            else if (line.find("location") != std::string::npos)
            {
                if (locblock)
                    config.locations.push_back(currloc);
                currloc = location_t(config);
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "location")
                    currloc.alias = tokens[1];
                locblock = 1;
            }
            else if (line.find("root") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "root")
                    currloc.root = tokens[1];
            }
            else if (line.find("allowed_method") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                for (size_t i = 1;  i< tokens.size(); ++i)
                {
                    std::cout << "allowed method token : " << tokens[i] << std::endl;
                    std::string  method = toUpper(tokens[i]);
                    if (method == "GET")
                        currloc.allow.push_back(1);
                    else if (method == "POST")
                        currloc.allow.push_back(2);
                    else if (method == "DELETE")
                        currloc.allow.push_back(3); 
               }
            }
            else if (line.find("index_auto") != std::string::npos)
            {
                if (line.find("on") != std::string::npos)
                    currloc.index_auto = true;
                else
                    currloc.index_auto = false;
            }
            else if (line.find("index") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                for (size_t i = 1; i < tokens.size(); ++i)
                    currloc.index.push_back(tokens[i]);
            }

        }
    }
    return config;
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
