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