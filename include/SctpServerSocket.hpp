#ifndef SCTPSERVERSOCKET_HPP
#define SCTPSERVERSOCKET_HPP

#include "SctpSocket.hpp"
#include "SctpClientSocket.hpp"

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
  auto listen(Lua::State*) noexcept -> int;
  auto accept(Lua::State*) noexcept -> int;
};

template<int IPV>
auto Server<IPV>::listen(Lua::State* L) noexcept -> int {
  int backLogSize = Lua::Aux::OptInteger(L, 2, Server<IPV>::DefaultBackLogSize);
  int result = ::listen(this->fd, backLogSize);
  if(result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "listen: %s", std::strerror(errno));
    return 2;
  }
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPV>
auto Server<IPV>::accept(Lua::State* L) noexcept -> int {
  int newFD = ::accept(this->fd, nullptr, nullptr);
  if(newFD < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
    //Non-blocking socket is being used, nothing to do
    Lua::PushBoolean(L, false);
    Lua::PushString(L, "accept: EAGAIN/EWOULDBLOCK");
    return 2;
  } else if(newFD < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "accept() failed: %s", std::strerror(errno));
    return 2;
  }
  auto connSock = Lua::NewUserData(L, sizeof(Sctp::Socket::Client<IPV>));
  if(connSock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }

  new (connSock) Sctp::Socket::Client<IPV>(newFD);

  Lua::Aux::GetMetaTable(L, Sctp::Socket::Client<IPV>::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

} //namespace Socket

} //namespace Sctp


#endif /* SCTPSERVERSOCKET_HPP */

