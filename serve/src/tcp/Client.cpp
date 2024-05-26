#include "Client.hpp"

Client::Client(int fd, Server& connect_server)
    : action(NULL), srv(connect_server), _clientfd(fd), _errorid(FALSE) {
  return;
}

Client::~Client() { close(_clientfd); }

int Client::getfd() const { return _clientfd; }

int* Client::clientptr() { return static_cast<int*>(&_clientfd); }


bool Client::errorid()
{
  return _errorid;
}

void Client::checkError( bool val )
{
  _errorid = val;
}

void Client::request() {
  char buf[SIZE_BUF];
  ssize_t byte = recv(_clientfd, buf, SIZE_BUF, 0);
  std::clog << "\nbyte-----\n" << byte << std::endl;
  std::clog << "\nbuf-----\n" << buf << std::endl;

  if (byte <= 0) {
    srv.disconnect(_clientfd); // ? 
    return ;
    } 
    else {
    try {
      if (Transaction::recvMsg(in, buf,
                               byte)) {  // cgi인지 아닌 지 판단해줌 ( header에
                                         // 있음  => fork했으면 pid 값이 나옴)
        logging.fs << in.msg.str() << std::endl;

        if (!action) {
          action = new Transaction(*this);

          if (subprocs.pid) {
            std::clog << "pid : " << subprocs.pid << std::endl;
            srv.setEvent(subprocs.pid, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
            0, 10000, clientptr());
            srv.setEvent(subprocs.pid, EVFILT_PROC, EV_ADD | EV_ONESHOT,
                         NOTE_EXIT, 0, clientptr());
            // void포인터 -> pid ident. client_socket이 들어가야되는데......
            // NOTE_EXIT -> event process가 종료될 때 이벤트를 발생한다?
          }
        }

        if (Transaction::recvBody(in, subprocs, buf, byte)) {
          logging.fs << in.body.str() << std::endl;

          if (!subprocs.pid) {
            if (action) action->act();
            in.reset();
            srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0,
                         NULL);
            if ( srv.timing == TRUE )
            {
              srv.setEvent(_clientfd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
              srv.timing = FALSE;
            }
          } else
            close(subprocs.fd[W]);  // pipe 가 있을 때 write event 지워줌
        }
      }
    } catch (errstat_t& err) {
      log("HTTP\t: transaction: " + str_t(err.what()));

      in.reset();
      out.reset();

      Transaction::buildError(err.code, *this);
      checkError(TRUE);
      action = NULL;
      if (subprocs.pid)
        srv.setEvent(subprocs.pid, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
      //이 부분도 timer있어야하는 거 아닌가???
      srv.setEvent(_clientfd, EVFILT_READ, EV_DELETE | EV_ONESHOT, 0, 0, NULL); 
      srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
      srv.setEvent(_clientfd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    }
    catch (err_t& err) {
      log("HTTP\t: Request: " + str_t(err.what()));
      in.reset();
      Transaction::buildError(400, *this);
      checkError(TRUE);
      action = NULL;
      if (subprocs.pid)
        srv.setEvent(subprocs.pid, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
      //이 부분도 timer있어야하는거아닌가?
      srv.setEvent(_clientfd, EVFILT_READ, EV_DELETE | EV_ONESHOT, 0, 0, NULL);
      srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
      srv.setEvent(_clientfd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    }
  }
}

bool Client::respond() {
  size_t length = out.msg.str().size();

  log("TCP\t: sending\n");
  logging.fs << out.msg.str() << "\n" << std::endl;

  ssize_t bytesSent = send(_clientfd, out.msg.str().c_str(), length, 0);
  if (bytesSent < 0) return false;
  if (out.body.str().size()) {
    length = out.body.str().size();
    logging.fs << out.body.str() << "\n" << std::endl;
    bytesSent = send(_clientfd, out.body.str().c_str(), length, 0);
    if (bytesSent < 0) return false;
  }

  out.reset();
  if (action != NULL && action->connection() == 1)
  {
    srv.disconnect(_clientfd);
    return false;
  }
  if (action) {
    delete action;
    action = NULL;
  }

  return true;
}

msg_buffer_s::msg_buffer_s() { reset(); }

void msg_buffer_s::reset() {
  msg.str("");
  msg.clear();

  msg_done = false;
  msg_read = 0;

  body.str("");
  body.clear();

  body_size = 0;
  body_read = 0;

  chunk = FALSE;
  incomplete = 0;
}

process_s::process_s() { reset(); }

void process_s::reset() {
  pid = NONE;
  stat = NONE;
  fd[R] = NONE;
  fd[W] = NONE;

  argv.clear();
  env.clear();
}

const msg_buffer_t& Client::buffer() const { return in; }

msg_buffer_t& Client::get_in() { return in; }
// process_t& Client::get_process() { return subprocs; }

const Server& Client::server() const { return srv; }