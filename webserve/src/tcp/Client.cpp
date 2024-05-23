#include "Client.hpp"

Client::Client(int fd, Server& connect_server)
    : action(NULL), srv(connect_server), _clientfd(fd) {
  (void)fd;
  return;
}

Client::~Client() { close(_clientfd); }

int Client::getfd() const { return _clientfd; }

void Client::request() {
  ssize_t byte = recv(_clientfd, buf, SIZE_BUF, 0);

  std::clog << "-->POST2<--" << std::endl;
  if (byte <= 0) {
    std::clog << "[REQUEST ERR_BYTE] : " << byte << std::endl;
    srv.disconnect(_clientfd);
    throw err_t("[REQUEST] : Falied to read/ [*ERR_BYTE*] [CLIENT->SERVER]");
  } else {
    try {
      std::clog << "-->POST3<--" << std::endl;
      if (Transaction::recvMsg(in, buf, byte)) {
        std::clog << "-->POST4<--" << std::endl;
        logging.fs << in.msg.str() << std::endl;

        if (!action) {
          std::clog << "IN ACTION<---\n";
          action = new Transaction(*this);
          std::clog << "--->NOT ACTION" << std::endl;
          // if (subprocs.pid) {
          //   srv.add_events(subprocs.pid, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
          //   0,
          //                  30000, get_client_socket_ptr());
          //   srv.add_events(subprocs.pid, EVFILT_PROC, EV_ADD | EV_ONESHOT,
          //                  NOTE_EXIT, 0, get_client_socket_ptr());
          // }
        }
        std::clog << "-->POST5<--" << std::endl;

        if (Transaction::recvBody(in, subprocs, buf, byte)) {
          logging.fs << in.body.str() << std::endl;

          // if (!subprocs.pid) {
          if (action) action->act();
          std::clog << "[ACT] : check if it's in" << std::endl;
          in.reset();
          srv.setEvent(_clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0,
                       NULL);
          std::clog << "[SET COMPLETE]\n";
          // }
          // else
          // close(subprocs.fd[W]);
        }
      }
    }
    // catch (errstat_t& err) {
    //   Transaction::buildError(err.code, *this);
    //   respond();
    // }

    catch (const std::exception& e) {
      std::clog << "ERROR\n";
      std::cerr << e.what() << '\n';
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
  // pid		= NONE;
  stat = NONE;
  // fd[R]	= NONE;
  // fd[W]	= NONE;

  argv.clear();
  env.clear();
}

const msg_buffer_t& Client::buffer() const { return in; }

msg_buffer_t& Client::get_in() { return in; }
// process_t& Client::get_process() { return subprocs; }

const Server& Client::server() const { return srv; }