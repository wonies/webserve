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

class NetClient
{
    public:
        NetClient();
        NetClient(int fd);
        ~NetClient();
        
    private:
        int _clientfd;
        std::ostringstream oss;

};


#endif