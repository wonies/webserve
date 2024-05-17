#include "filter.hpp"
#include "structure.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

config_s::config_s()
    : client_max_body(0) {}

// location_s의 생성자 정의
location_s::location_s(const config_s& config)
    : index_auto(false) {}

struct isNotSpace {
    bool operator()(unsigned char ch) const {
        return !std::isspace(ch);
    }
};

std::string readfile(const std::string &filepath)
{
    std::ifstream file("default.config");
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: ");
    file.seekg(0, std::ios::end);
    std::size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    // 파일의 내용을 문자열로 읽습니다.
    std::string content;
    content.resize(length);
    file.read(&content[0], length);
    file.close();
    std::cout << content << std::endl;
    std::cout << "config setting\n";
    return content;
}

void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), isNotSpace()));
    s.erase(std::find_if(s.rbegin(), s.rend(), isNotSpace()).base(), s.end());
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        trim(token);  // 각 토큰의 앞뒤 공백을 제거
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

config_t parse(const std::string &content)
{
    std::istringstream stream(content);
    std::ostringstream oss;

    std::string line;
    config_t config;
    location_t currentLocation(config);
    bool inLocation = false;
    bool serverBlock = false;
    bool locationBlock = false;

    while(std::getline(stream, line))
    {
        trim(line);
        if (line.empty())
            continue ;
        if (line == "server {")
        {
            if (serverBlock == 1)
                throw std::runtime_error("Nested server blocks are not allowed");
            config = config_t();
            serverBlock = 1;
            oss.str("");
        }
        else if (line == "}")
        {
            if (locationBlock)
            {
                config.locations.push_back(currentLocation);
                locationBlock = 0;
            }
            if (serverBlock)
            {
                std::cout << "server block end  : " << std::endl;
                std::cout << "server name : " << config.name << std::endl;
                std::cout << "listen port : " << config.listen << std::endl;
                std::cout << "client max body size : " << config.client_max_body << std::endl;
                for (size_t i = 0; i < config.locations.size(); ++i)
                {
                    const location_t& loc = config.locations[i];
                    std::cout << "location : " << i << ": " << std::endl;
                    std::cout << "alias : " << loc.alias << std::endl;
                    std::cout << "root : " << loc.root << std::endl;
                    std::cout << "index auto : " << loc.index_auto << std::endl;
                    std::cout << "allow : ";
                    for (size_t j = 0; j <loc.allow.size(); ++j)
                        std::cout << loc.allow[j] << " ";
                    std::cout << std::endl; 
                }
                serverBlock = 0;
            }
        }
        else if (serverBlock)
        {
            oss << line << "\n";
            if (line.find("listen") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "listen")
                {
                    try {
                        size_t pos;
                        int port = std::stoi(tokens[1], &pos);
                        if (pos != tokens[1].size())
                            throw std::invalid_argument("Invalid port number");
                        config.listen = port;
                        std::cout << "tokens : " << tokens[1];
                        std::cout << std::endl;
                    } catch (const std::exception &e) {
                        throw std::runtime_error("Invalid 'listen' directive: " + tokens[1]);
                    }
                }
                else 
                    throw std::runtime_error("Invalid 'listen' directive: " + tokens[1]);

                //     config.listen = std::atoi(tokens[1].c_str());
                // else 
                //     std::cout << "unexpected error : listen error\n";
            }
            else if (line.find("server_name") != std::string::npos)
            {
               std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "server_name")
                {
                    std::string serverName = tokens[1];
                    for (size_t i = 0; i < serverName.size(); ++i) {
                        if (!std::isalnum(serverName[i]) && serverName[i] != '.' && serverName[i] != '-') {
                            throw std::runtime_error("Invalid 'server_name' directive: " + serverName);
                        }
                    }
                     for (std::size_t i = 1; i < tokens.size(); ++i) {
                        std::cout << "token name : " << tokens[i] << std::endl;
                    }
                    // std::cout << "server_name: " << config.name << std::endl;
                    config.name = serverName;
                }
                else
                    throw std::runtime_error("Invalid server");
            }
            else if (line.find("Client_body_size") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1 && tokens[0] == "Client_body_size")
                {
                    std::string  sizestr = tokens[1];
                    size_t ssize = std::atoi(sizestr.substr(0, sizestr.size() -1).c_str());
                    char unit =  sizestr.back();
                    // if (unit == 'M' || unit == 'm')
                    //     ssize *= (1024 * 1024);
                config.client_max_body = ssize;
                }
                else
                    throw std::runtime_error("Invalid client body size");
            }
            else if (line.find("location")!= std::string::npos)
            {
                if (locationBlock)
                    config.locations.push_back(currentLocation);
                currentLocation = location_t(config);
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1)
                    currentLocation.alias = tokens[1];
                locationBlock = true;
            }
            else if (line.find("root") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                if (tokens.size() > 1) {
                    currentLocation.root = tokens[1];
            }
            else if (line.find("allowed_method") != std::string::npos)
            {
                  std::vector<std::string> tokens = split(line, ' ');
                for (std::size_t i = 1; i < tokens.size(); ++i) {
                    if (tokens[i] == "GET") currentLocation.allow.push_back(1);
                    else if (tokens[i] == "POST") currentLocation.allow.push_back(2);
                    else if (tokens[i] == "DELETE") currentLocation.allow.push_back(3);
            }
            }
            else if (line.find("index_auto") != std::string::npos)
            {
                if (line.find("on") != std::string::npos)
                    currentLocation.index_auto = true;
                 else 
                    currentLocation.index_auto = false;
            }
            else if (line.find("index") != std::string::npos)
            {
                std::vector<std::string> tokens = split(line, ' ');
                for (std::size_t i = 1; i < tokens.size(); ++i) {
                    currentLocation.index.push_back(tokens[i]);
            }
            }
            }
        }
    }
    return config;
}

// config_t parse(const std::string &content)
// {
//     std::ostringstream oss;
//     std::istringstream stream(content);
//     std::string line;
//     config_t config;
//     location_t currentLocation(config);
//     bool inServerBlock = false;
//     bool inLocationBlock = false;

//     while (std::getline(stream, line)) {
//         trim(line);
//         if (line.empty() || line[0] == '#') {
//             continue;
//         }

//         if (line == "server {") {
//             if (inServerBlock) {
//                 throw std::runtime_error("Nested server blocks are not allowed");
//             }
//             inServerBlock = true;
//             config = config_t();
//             oss.str("");  // 문자열 스트림을 비웁니다.
//         } else if (line == "}") {
//             if (inLocationBlock) {
//                 config.locations.push_back(currentLocation);
//                 inLocationBlock = false;
//             }
//             if (inServerBlock) {
//                 // 서버 블록이 끝났을 때 config를 출력
//                 std::cout << "Server block end:" << std::endl;
//                 std::cout << "  Server name: " << config.name << std::endl;
//                 std::cout << "  Listen port: " << config.listen << std::endl;
//                 std::cout << "  Client max body size: " << config.client_max_body << std::endl;
//                 for (size_t i = 0; i < config.locations.size(); ++i) {
//                     const location_t& loc = config.locations[i];
//                     std::cout << "  Location " << i << ":" << std::endl;
//                     std::cout << "    Alias: " << loc.alias << std::endl;
//                     std::cout << "    Root: " << loc.root << std::endl;
//                     std::cout << "    Index auto: " << (loc.index_auto ? "on" : "off") << std::endl;
//                     std::cout << "    Allow: ";
//                     for (size_t j = 0; j < loc.allow.size(); ++j) {
//                         std::cout << loc.allow[j] << " ";
//                     }
//                     std::cout << std::endl;
//                 }
//                 inServerBlock = false;
//             } else {
//                 throw std::runtime_error("Unexpected '}' without a matching block");
//             }
//         } else if (inServerBlock) {
//             oss << line << '\n';  // 라인을 문자열 스트림에 추가합니다.
//             if (line.find("listen") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 if (tokens.size() > 1) {
//                     config.listen = std::atoi(tokens[1].c_str());
//                 }
//             } else if (line.find("server_name") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 if (tokens.size() > 1) {
//                     config.name = tokens[1];
//                 }
//             } else if (line.find("Client_body_size") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 if (tokens.size() > 1) {
//                     std::string sizeStr = tokens[1];
//                     size_t size = std::atoi(sizeStr.substr(0, sizeStr.size() - 1).c_str());
//                     char unit = sizeStr.back();
//                     if (unit == 'K' || unit == 'k') {
//                         size *= 1024;
//                     } else if (unit == 'M' || unit == 'm') {
//                         size *= 1024 * 1024;
//                     }
//                     config.client_max_body = size;
//                 }
//             } else if (line.find("location") == 0) {
//                 if (inLocationBlock) {
//                     config.locations.push_back(currentLocation);
//                 }
//                 currentLocation = location_t(config);
//                 std::vector<std::string> tokens = split(line, ' ');
//                 if (tokens.size() > 1) {
//                     currentLocation.alias = tokens[1];
//                 }
//                 inLocationBlock = true;
//             } else if (line.find("root") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 if (tokens.size() > 1) {
//                     currentLocation.root = tokens[1];
//                 }
//             } else if (line.find("allowed_method") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 for (std::size_t i = 1; i < tokens.size(); ++i) {
//                     if (tokens[i] == "GET") currentLocation.allow.push_back(1);
//                     else if (tokens[i] == "POST") currentLocation.allow.push_back(2);
//                     else if (tokens[i] == "DELETE") currentLocation.allow.push_back(3);
//                 }
//             } else if (line.find("index_auto") == 0) {
//                 if (line.find("on") != std::string::npos) {
//                     currentLocation.index_auto = true;
//                 } else {
//                     currentLocation.index_auto = false;
//                 }
//             } else if (line.find("index") == 0) {
//                 std::vector<std::string> tokens = split(line, ' ');
//                 for (std::size_t i = 1; i < tokens.size(); ++i) {
//                     currentLocation.index.push_back(tokens[i]);
//                 }
//             }
//         }
//     }

//     std::cout << "Parsed content:\n" << oss.str();  // 파싱된 내용을 출력합니다.
//     return config;
// }



int main()
{
    std::string content = readfile("default.config");
    config_t config = parse(content);
}

