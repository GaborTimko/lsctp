#ifndef SCTPSERVERSOCKET_HPP
#define SCTPSERVERSOCKET_HPP

#include "SctpSocket.hpp"
#include "SctpClientSocket.hpp"

namespace Sctp {

namespace Socket {

template<int IPVersion>
class Server final : public Base<IPVersion> {
public:
  static constexpr int DefaultBackLogSize = 1000;
  static const char* MetaTableName;
public:
  Server() : Base<IPVersion>() {}
public:
  auto listen(Lua::State*) noexcept -> int;
  auto accept(Lua::State*) noexcept -> int;
};

template<int IPVersion>
auto Server<IPVersion>::listen(Lua::State* L) noexcept -> int {
  int backLogSize = Lua::Aux::OptInteger(L, 2, Server<IPVersion>::DefaultBackLogSize);
  int result = ::listen(this->fd, backLogSize);
  if(result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "listen: %s", std::strerror(errno));
    return 2;
  }
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPVersion>
auto Server<IPVersion>::accept(Lua::State* L) noexcept -> int {
  int newFD = ::accept(this->fd, nullptr, nullptr);
  if(newFD < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
    //Non-blocking socket is being used, nothing to do
    Lua::PushBoolean(L, false);
    Lua::PushString(L, "EAGAIN/EWOULDBLOCK");
    return 2;
  } else if(newFD < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "accept() failed: %s", std::strerror(errno));
    return 2;
  }
  auto connSock = Lua::NewUserData<Sctp::Socket::Client<IPVersion>>(L);
  if(connSock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }

  new (connSock) Sctp::Socket::Client<IPVersion>(newFD);

  Lua::Aux::GetMetaTable(L, Sctp::Socket::Client<IPVersion>::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

} //namespace Socket

} //namespace Sctp


#endif /* SCTPSERVERSOCKET_HPP */

