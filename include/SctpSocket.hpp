#ifndef LSSOCKET_HPP
#define LSSOCKET_HPP

#include <system_error>

namespace Sctp {

class Socket {
private:
  int fd;
public:
  Socket(int IPVersion);
};

} //namespace Sctp

#endif /* LSSOCKET_HPP */

