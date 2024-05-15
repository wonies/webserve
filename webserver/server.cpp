#include "server.hpp"


NetServer::NetServer()
{
    initSocket();
    setupBind();
    startListening();
}

NetServer::~NetServer()
{
    close(sock);
}

void NetServer::initSocket()
{
    if ((sock = socket(PF_INET, SOCK_STREAM, 0))==-1)
        exit_error("socket error");
}

void NetServer::setupBind()
{
    struct sockaddr_in saddr;
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(8080);
    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr))==-1)
        exit_error("bind error");
    fcntl(ssocket, F_SETFL, O_NONBLOCK); //non-blocking 
}

void NetServer::startListening()
{
    if (listen(sock, 10)==-1)
        exit_error("listen error");
}


void NetServer::exit_error(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

void NetServer::createKqueue()
{
    if((kq = kqueue())==-1);
        exit_error("kqueue error");
}

void NetServer::serverEvent()
{
    enrollEvent(sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void NetServer::enrollEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent kev;
    EV_SET(&kev, ident, filter, flags, fflags, data, udata);
    if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
        exit_error("Failed to register event with kqueue");
}

void NetServer::manageEvent()
{
    while (1)
    {
        nevents = kevent(kq, NULL, 0, &events, 10, NULL);
        if (nevents == -1)
            exit_error("event manage Error");
        for (int i=0; i<nevents; ++i)
        {
            struct kevent temp = events[i];
            if (temp->filter == EVFILT_READ)
            {
                if (temp->ident == sock)
                {
                    NetClient newClient();
                    if (NetClient._clientfd = accept(sock, NULL, NULL) == -1)
                        exit_error("client create error");
                    clients[Netclient._clientfd] = newClient;
                    enrollEvent(NetClient._clientfd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                    fcntl(ident, F_SETFL, O_NONBLOCK);
                }
                else
                {
                    if (newClient.receiving() == 1)
                        clients[NetClient._clientfd] += newClient.oss; 
                    else
                    {
                        clients.erase(newClient._clientfd);
                        exit_error("receive error");
                    }
                }
            }

        }
    }
}