#include "client.hpp"

Netclient::Netclient() :_clientfd(-1)
{
    return ;
}

NetClient::NetClient(int fd) : _clientfd(fd) {}

NetClient::~NetClient()
{
    close(_clientfd);
}

bool NetClient::receiving()
{
    char buf[1024];
    int rec = recv(_clientfd, buf, 1024, 0);
    if (rec > 0)
    {
        buf[rec] = '\0';
        oss << buf;
        return true;
    }   
    else if (rec <= 0)
    {
        close(_clientfd);
        return false;
    }   
}

// bool NetClient::writing()
// {
//     int sent = send(_clientfd,);
// }