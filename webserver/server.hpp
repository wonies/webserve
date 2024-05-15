#ifndef NET_SERVER_H
#define NET_SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "client.hpp"

class NetClient;

class NetServer
{
    public:
        NetServer();
        ~NetServer();
        void initSocket();
        void setupBind();
        void startListening();
        void enterKqueue();
        void exit_error(const std::string &msg);
        void createKqueue();
        void enrollEvent();
        void manageEvent();
    private:
        int sock;
        int kq;
        int nevents;
        struct kevent events[10];
        std::map<int, NetClient> clients;

};





#endif