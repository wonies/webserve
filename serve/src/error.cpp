#include "error.hpp"

void throwSysErr(const str_t& fncName, uint_t code) {
  perror(fncName.c_str());
  throw errstat_t(code);
}

errstat_s::errstat_s(const uint_t& status) : err_t("something gone wrong") {
  std::clog << "check status" << status << std::endl;
  code = status;
}
errstat_s::errstat_s(const uint_t& status, const str_t& msg) : err_t(msg) {
  code = status;
}