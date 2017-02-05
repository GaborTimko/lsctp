#ifndef LSSOCKET_HPP
#define LSSOCKET_HPP

#include <system_error>
#include <vector>
#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Lua/Lua.hpp"

namespace Sctp {

constexpr int IPversionToAddrFamily(int ipv) {
  return ipv == 4 ? AF_INET : AF_INET6;
}

template<int IPV>
class Socket {
  static_assert(IPV == 4 or IPV == 6, "Invalid IP version");
private:
  using SockAddrType = std::conditional_t<IPV == 4, sockaddr_in, sockaddr_in6>;
private:
  int fd;
  bool roleIsServer;
  bool haveBoundAddresses;
  std::vector<SockAddrType> addresses;
public:
  Socket();
public:
  auto bind(Lua::State*) -> int;
  auto close(Lua::State*) -> int;
private:
  auto bindFirst(Lua::State*) -> int;
  auto pushIPAddress(Lua::State*, const char* ip, uint16_t port, int idx) -> int;
  auto checkIPConversionResult(Lua::State*, const char* ip, int result) -> int;
};

template<int IPV>
inline Socket<IPV>::Socket() : haveBoundAddresses(false) {
  static_assert(IPV == 4 or IPV == 6, "Invalid IP version");

  fd = ::socket(IPversionToAddrFamily(IPV), SOCK_STREAM, IPPROTO_SCTP);
  if(fd == -1) {
    throw std::system_error(errno, std::system_category());
  }
  int True = 1;
  setsockopt(fd, IPPROTO_SCTP, SO_REUSEADDR, &True, sizeof(int));
}

//TODO: also handle table input for the addresses
template<int IPV>
auto Socket<IPV>::bind(Lua::State* L) -> int {
  uint16_t port = htons(Lua::ToInteger(L, 2));
  int stackSize = Lua::GetTop(L);
  std::size_t addrCount = stackSize - 2;
  if(addrCount < 1) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, "No addresses were given");
    return 2;
  }
  addresses.resize(addrCount);
  std::memset(addresses.data(), 0, sizeof(SockAddrType) * addrCount);
  for(int i = 3; i <= stackSize; i++) {
    int idx = i - 3;
    auto addrI = Lua::ToString(L, i);
    int pushRes = pushIPAddress(L, addrI, port, idx);
    if(pushRes > 0) {
      return pushRes;
    }
  }

  int retVal = bindFirst(L);
  if(retVal > 0) {
    return retVal;
  }

  if(addrCount > 1) {
    int bindRes = ::sctp_bindx(fd, reinterpret_cast<sockaddr*>(addresses.data() + 1), addrCount - 1, SCTP_BINDX_ADD_ADDR);
    if(bindRes < 0) {
      Lua::PushBoolean(L, false);
      Lua::PushString(L, std::strerror(errno));
      return 2;
    }
  }

  haveBoundAddresses = true;
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPV>
auto Socket<IPV>::bindFirst(Lua::State* L) -> int {
  int bindRes = 0;
  if(IPV == 4) {
    bindRes = ::bind(fd, reinterpret_cast<sockaddr*>(addresses.data()), sizeof(SockAddrType));
  } else {
    bindRes = ::bind(fd, reinterpret_cast<sockaddr*>(addresses.data()), sizeof(SockAddrType));
  }

  if(bindRes < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  return bindRes;
}

template<>
inline auto Socket<4>::pushIPAddress(Lua::State* L, const char* ip, uint16_t port, int idx) -> int {
  addresses[idx].sin_family = AF_INET;
  addresses[idx].sin_port   = port;
  int conversion = ::inet_pton(AF_INET, ip, &addresses[idx].sin_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<>
inline auto Socket<6>::pushIPAddress(Lua::State* L, const char* ip, uint16_t port, int idx) -> int {
  addresses[idx].sin6_family = AF_INET6;
  addresses[idx].sin6_port   = port;
  int conversion = ::inet_pton(AF_INET6, ip, &addresses[idx].sin6_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<int IPV>
auto Socket<IPV>::checkIPConversionResult(Lua::State* L, const char* ip, int result) -> int {
  if(result == 0) {
    char errorStr[100] = "Invalid IP: ";
    std::strcat(errorStr, ip);
    Lua::PushBoolean(L, false);
    Lua::PushString(L, errorStr);
    return 2;
  } else if (result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  return 0;
}

template<int IPV>
auto Socket<IPV>::close(Lua::State* L) -> int {
  if(fd > -1 and ::close(fd) < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    fd = -1;
    return 2;
  }
  fd = -1;
  Lua::PushBoolean(L, true);
  return 1;
}

} //namespace Sctp

#endif /* LSSOCKET_HPP */

