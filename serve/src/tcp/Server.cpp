#include "Server.hpp"

Server::Server(const vec_config_t &confs) :timing(FALSE), events(10),  _confs(confs) {
  preset();
}

Server::~Server() {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }
  close(sock);
  close(kq);
}

void Server::preset() {
  _time.tv_sec = 100;
  _kqueue();
}

void Server::_kqueue() {
  if ((kq = kqueue()) == -1) throw err_t(" [EVENT] : Failed to create kqueue");
}

void Server::_server(int port) {
  init(port);
  setEvent(sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  _type[sock] = "serv";  // socket도 배열화 해야하는 건 아닌가?
}

void Server::setEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void *udata) {
    struct kevent kev;
    EV_SET(&kev, ident, filter, flags, fflags, data, udata);
    
    // 자세한 로그 출력
    std::clog << "[EV] ident: " << ident
              << ", filter: " << filter
              << ", flags: " << flags
              << ", fflags: " << fflags
              << ", data: " << data
              << ", udata: " << udata
              << std::endl;
    // kevent 호출
    if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
        std::cerr << "Failed to register events: " << strerror(errno) << std::endl;
    } else {
        std::clog << "Event registered successfully: ident=" << ident
                  << ", filter=" << filter << std::endl;
    }
}

void Server::disconnect(int) {
  std::map<int, Client *>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    close(fd);
    delete it->second;
    clients.erase(it);
  }
}

void Server::addClient(int fd) {
  Client *newClient = new Client(fd, *this);
  clients[fd] = newClient;
  _type[fd] = "cl";
  nonblock(fd);
}

void Server::connectEvent() {
  int fd;
  if ((fd = accept(sock, NULL, NULL)) == -1)
    throw err_t("Failed to accept socket fd");
  addClient(fd);
  setEvent(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  setEvent(fd, EVFILT_TIMER, EV_ADD, 0, 30000, NULL);
  timing= TRUE;
}

void Server::clientEvent(struct kevent &event) {
  std::map<int, Client *>::iterator it = clients.find(event.ident);
  if (it != clients.end()) {
    Client *tmp = it->second;
    tmp->request();
  } else
    std::clog << "Client not FOUND : " << event.ident << std::endl;
}

bool Server::eventerr(struct kevent &event) {
  if (event.flags & EV_ERROR) {
    disconnect(event.ident);
    return true;
  }
  if ((event.flags & EV_EOF) && (event.filter != EVFILT_PROC)) {
    std::map<int, Client *>::iterator it = clients.find(event.ident);
    if (it != clients.end()) {
      disconnect(event.ident);
      return true;
    }
  }
  return false;
}

int Server::_cnttake() {
  int nevent;
  nevent = kevent(kq, NULL, 0, &events[0], 10, NULL);
  if (nevent == -1) throw("Failed to take place a event");
  return nevent;
}

void Server::_read(struct kevent &event) {
  int id = event.ident;
  if (_type[id] == "serv")
    connectEvent();
  else if (_type[id] == "cl")
    clientEvent(event);
}

void Server::_write(struct kevent &event) {
  int fds = event.ident;
  std::map<int, Client *>::iterator it = clients.find(fds);
  Client &_client = *it->second;
  if (it != clients.end()) {
    if (!(_client.respond())) {
      disconnect(fds);
      std::clog << "_write disconnect" << std::endl;
      return;
    }
  }
  if (_client.subprocs.pid && _client.errorid())
  {
    std::clog << "----kill------;\n";
    kill(_client.subprocs.pid,SIGTERM);
    _client.checkError( FALSE );
    return ;
  }
  if ((!(_client.subprocs.pid)) && _client.errorid())
  {
    disconnect(fds);
    return ;
  }
  if (_client.action != NULL && _client.action->connection() == 1)
  {
    disconnect(fds);
    return ;
  }
  
}

void Server::_proc(struct kevent &event) {
  // 1. client socket을 찾음
  // 2. 김동현 COPY
  std::clog << "event . ident : " << event.ident << std::endl;
  int client = *(static_cast<int *>(event.udata));
  std::clog << "client fd : " << client << std::endl;
  std::map<int, Client *>::iterator it = clients.find(client);
  Client &cl = *it->second;
  try {
    CGI::wait(cl.subprocs);
    // it->second->setCgiExit(TRUE);
    if (WEXITSTATUS(cl.subprocs.stat) != EXIT_SUCCESS) {
      throw errstat_t(500, "the CGI failed to exit as SUCCESS");
    }
    CGI::readFrom(cl.subprocs, cl.out.body);
    CGI::build(cl.out);

    cl.in.reset();
    cl.subprocs.reset();
    // std::clog << event.ident << std::endl;
    setEvent(event.ident, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    // setEvent(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    // setEvent(client, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    setEvent(client, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
  }

  catch (const errstat_t &e) {
    log("TCP\t: " + str_t(e.what()));

    cl.out.reset();
    close(cl.subprocs.fd[R]);
    Transaction::buildError(e.code, cl);
    cl.checkError(TRUE);

    // setEvent(cl.subprocs.pid, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    // setEvent(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    setEvent(client, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
  }
}


void Server::_timer(struct kevent &event)
{
// timer가 끝났을 때.. 이상이 있다.... 무슨 이상이 있을까? 30초가 들어와서 온거니까...
  int udata = 0;
  int fd = event.ident;
  if ( event.udata != NULL )
    udata = *(static_cast<int *>( event.udata ));
  std::map<int, Client *>::iterator it = clients.find( fd );
  try
  {
    if (it != clients.end())
    {
      throw errstat_t( 503, "Time out request on HTTP" );
    }
    else if (event.udata != NULL)
    {
      setEvent( event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL );
      setEvent( event.ident, EVFILT_TIMER,EV_DELETE, 0, 0, NULL );
      throw errstat_t( 503, "Time out request IN CGI" );
    }
  }
  catch (const errstat_t& e) {
		log( "TCP\t: " + str_t( e.what() ) );
    if (it != clients.end()) 
    {    
      Client& cl = *it->second;
      cl.checkError(TRUE);
      cl.out.reset();
      Transaction::buildError( e.code, cl );
      setEvent( event.ident, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL );
      return ;
    }
    else if (event.udata != NULL) 
    {
      it = clients.find( udata );
      if (it != clients.end() && it->second != NULL) {
        Client& cl = *it->second;
        cl.checkError(TRUE);
        if (cl.subprocs.pid) {
          cl.out.reset();
          Transaction::buildError( e.code, cl );
      }
      setEvent( udata, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL );
      }
    }
  }
}




void Server::confset(vec_config_t &conf) {
  vec_config_t::iterator it = conf.begin();
  vec_config_t::iterator endit = conf.end();
  for (; it != endit; ++it) {
    _server(it->listen);
  }
}

void Server::run(vec_config_t &conf) {
  confset(conf);
  while (true) {
    nevents = _cnttake();
    for (int i = 0; i < nevents; ++i) {
      struct kevent &temp = events[i];
      if (eventerr(temp)) continue;

      if (temp.filter == EVFILT_READ)
        _read(temp);
      else if (temp.filter == EVFILT_WRITE)
        _write(temp);
      else if (temp.filter == EVFILT_PROC)
        _proc(temp);
      else if (temp.filter == EVFILT_TIMER)
        _timer(temp);
    }
  }
}

/*
 언제 발생하는 지 -> 활동하고 끝날 때 , 발생하면
                             뭐 해야하는 지 // proc event 서버가 일을 맡겨놈
                             -> server 1. cgi요청만하고 다른 일함 -> 끝나야
                             일을 마저 진행함. 2. 끝나는 시점이 언제 인지
                             알려고  // 발생하면 응답을 만들어야 함 /
                             응답상태코드_ -> 응답이 잘 왔는 지 안왔는 지 /
                             send
// proc의 ident는 not socket, process의 pid임 -> pid
                      // proc이벤트 발생하면 어떤 프로세스가 죽었는 지만 알기에,
                      // 어떤 소켓이랑 연결된 이벤트인지 . 알수. 없음
                      // udata->socket udata
*/