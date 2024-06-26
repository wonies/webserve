#include "Client.hpp"

Client::Client(int fd, Server& connect_server)
    : action(NULL), srv(connect_server), _clientfd(fd) {
  (void)fd;
  return;
}

Client::~Client() { close(_clientfd); }

int Client::getfd() const { return _clientfd; }

int* Client::clientptr() { return static_cast<int*>(&_clientfd); }

void Client::request() {
  char buf[SIZE_BUF];
  ssize_t byte = recv(_clientfd, buf, SIZE_BUF, 0);

  if (byte <= 0) {
    srv.disconnect(_clientfd);
    throw err_t("[REQUEST] : Falied to read/ [*ERR_BYTE*] [CLIENT->SERVER]");
  } else {
    try {
      if (Transaction::recvMsg(in, buf,
                               byte)) {  // cgi인지 아닌 지 판단해줌 ( header에
                                         // 있음  => fork했으면 pid 값이 나옴)
        logging.fs << in.msg.str() << std::endl;

        if (!action) {
          action = new Transaction(*this);

          if (subprocs.pid) {
            std::clog << "pid : " << subprocs.pid << std::endl;
            // srv.add_events(subprocs.pid, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
            // 0,
            //                30000, get_client_socket_ptr());
            srv.setEvent(subprocs.pid, EVFILT_PROC, EV_ADD | EV_ONESHOT,
                         NOTE_EXIT, 0, clientptr());
            // void 향 포인터 -> pid ident. client_socket이 들어가야되는데......
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
          } else
            close(subprocs.fd[W]);  // pipe 가 있을 때 write event 지워줌
        }
        std::clog << "request pid :  " << subprocs.pid << std::endl;
      }
    } catch (errstat_t& err) {
      log("HTTP\t: transaction: " + str_t(err.what()));

      in.reset();
      out.reset();

      Transaction::buildError(err.code, *this);
      // setCgiCheck(TRUE);
      action = NULL;
      srv.setEvent(_clientfd, EVFILT_READ, EV_DELETE | EV_ONESHOT, 0, 0, NULL);
      srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
    }

    catch (err_t& err) {
      log("HTTP\t: Request: " + str_t(err.what()));

      in.reset();

      Transaction::buildError(400, *this);
      action = NULL;
      // setCgiCheck(TRUE);
      srv.setEvent(_clientfd, EVFILT_READ, EV_DELETE | EV_ONESHOT, 0, 0, NULL);
      srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
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