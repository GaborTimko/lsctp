#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include "SctpSocket.hpp"

namespace Sctp {

Socket::Socket(int IPVersion) {
  //The same code is generated with the ternary operator,
  //so I'll stick to the more readable one.
  if(IPVersion == 4) {
    IPVersion = AF_INET;
  } else if (IPVersion == 6) {
    IPVersion = AF_INET6;
  } else {
  	throw std::system_error(EINVAL, std::system_category(), "Address family not supported by this library");
  }

  fd = socket(IPVersion, SOCK_STREAM, IPPROTO_SCTP);
  if(fd == -1) {
    throw std::system_error(errno, std::system_category());
  }
}

}
