#ifndef SCTPSERVERSOCKET_HPP
#define SCTPSERVERSOCKET_HPP

#include "SctpSocket.hpp"

namespace Sctp {

namespace Socket {

template<int IPV>
class Server final : public Base<IPV> {
public:
  static constexpr int DefaultBackLogSize = 1000;
  static const char* MetaTableName;
public:
  Server() : Base<IPV>() {}
public:
  auto listen(Lua::State* L) noexcept -> int;
  auto accept() noexcept -> int;
};

template<int IPV>
auto Server<IPV>::listen(Lua::State* L) noexcept -> int {
  int backLogSize = Lua::Aux::OptInteger(L, 2, Server<IPV>::DefaultBackLogSize);
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
auto Server<IPV>::accept() noexcept -> int {
  int newFD = ::accept(this->fd, nullptr, nullptr);
  if(newFD < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
    //Non-blocking socket is being used, nothing to do
    return 0;
  }
  return newFD;
}

} //namespace Socket

} //namespace Sctp


#endif /* SCTPSERVERSOCKET_HPP */

