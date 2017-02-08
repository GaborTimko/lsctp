#ifndef SCTPSERVERSOCKET_HPP
#define SCTPSERVERSOCKET_HPP

#include "SctpSocket.hpp"

namespace Sctp {

template<int IPV>
class ServerSocket : public Socket<IPV> {
public:
  static constexpr int DefaultBackLogSize = 1000;
  static const char* MetaTableName;
public:
  ServerSocket() : Socket<IPV>() {}
public:
  auto listen(Lua::State* L) noexcept -> int;
  auto accept() noexcept -> int;
};

template<int IPV>
auto ServerSocket<IPV>::listen(Lua::State* L) noexcept -> int {
  int backLogSize = Lua::Aux::OptInteger(L, 2, ServerSocket<IPV>::DefaultBackLogSize);
  int result = ::listen(this->fd, backLogSize);
  if(result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPV>
auto ServerSocket<IPV>::accept() noexcept -> int {
  int newFD = ::accept(this->fd, nullptr, nullptr);
  if(newFD < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
    //Non-blocking socket is being used, nothing to do
    return 0;
  }
  return newFD;
}

} //namespace Sctp


#endif /* SCTPSERVERSOCKET_HPP */

