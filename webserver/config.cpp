#include "filter.hpp"
#include "structure.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


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




int main()
{
    std::string content = readfile("default.config");
    config_t config = parse(content);
}

