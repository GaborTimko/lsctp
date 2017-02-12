#ifndef SCTPCLIENTSOCKET_HPP
#define SCTPCLIENTSOCKET_HPP

#include "SctpSocket.hpp"

namespace Sctp {

namespace Socket {

template<int IPV>
class Client final : public Base<IPV> {
public:
  static const char* MetaTableName;
public:
  Client() : Base<IPV>() {}
  Client(int sock);
public:
  auto connect(Lua::State*) noexcept -> int;
};

template<int IPV>
Client<IPV>::Client(int sock) : Base<IPV>(sock) {}

template<int IPV>
auto Client<IPV>::connect(Lua::State* L) noexcept -> int {
  typename Base<IPV>::AddressArray peerAddresses;
  std::size_t addrCount;
  int loadAddrResult = this->loadAddresses(L, peerAddresses, addrCount);
  if(loadAddrResult > 0) {
    return loadAddrResult;
  }

  if(::sctp_connectx(this->fd, reinterpret_cast<sockaddr*>(peerAddresses.data()), addrCount, nullptr) < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "sctp_connectx: %s", std::strerror(errno));
    return 2;
  }

  Lua::PushBoolean(L, true);
  return 1;
}

} //namespace Socket

} //namespace Sctp

#endif /* SCTPCLIENTSOCKET_HPP */

