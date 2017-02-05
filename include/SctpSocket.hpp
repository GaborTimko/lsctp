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
public:
  using SockAddrType = std::conditional_t<IPV == 4, sockaddr_in, sockaddr_in6>;
public:
  static constexpr int DefaultBackLogSize = 1000;
private:
  int fd;
  bool roleIsServer;
  bool haveBoundAddresses;
  std::vector<SockAddrType> addresses;
public:
  Socket();
  Socket(int sock) noexcept;
  ~Socket();
public:
  auto bind(Lua::State*) noexcept -> int;
  auto listen(Lua::State* L) noexcept -> int;
  auto accept() noexcept -> int;
  auto close(Lua::State*) noexcept -> int;
private:
  auto bindFirst(Lua::State*) noexcept -> int;
  auto pushIPAddress(Lua::State*, const char* ip, uint16_t port, int idx) noexcept -> int;
  auto checkIPConversionResult(Lua::State*, const char* ip, int result) noexcept -> int;
};

template<int IPV>
inline Socket<IPV>::Socket() : roleIsServer(false), haveBoundAddresses(false) {
  static_assert(IPV == 4 or IPV == 6, "Invalid IP version");

  fd = ::socket(IPversionToAddrFamily(IPV), SOCK_STREAM, IPPROTO_SCTP);
  if(fd == -1) {
    throw std::system_error(errno, std::system_category());
  }
  int True = 1;
  setsockopt(fd, IPPROTO_SCTP, SO_REUSEADDR, &True, sizeof(int));
}

template<int IPV>
inline Socket<IPV>::Socket(int sock) noexcept : fd(sock), roleIsServer(false), haveBoundAddresses(false) {}

template<int IPV>
Socket<IPV>::~Socket() {
  if(fd > -1){
    ::close(fd);
  }
}

//TODO: also handle table input for the addresses
template<int IPV>
auto Socket<IPV>::bind(Lua::State* L) noexcept -> int {
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
auto Socket<IPV>::listen(Lua::State* L) noexcept -> int {
  int backLogSize = Lua::Aux::OptInteger(L, 2, Socket<IPV>::DefaultBackLogSize);
  int result = ::listen(fd, backLogSize);
  if(result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  roleIsServer = true;
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPV>
auto Socket<IPV>::accept() noexcept -> int {
  int newFD = ::accept(fd, nullptr, nullptr);
  if(newFD < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
    //Non-blocking socket is being used, nothing to do
    return 0;
  }
  return newFD;
}

template<int IPV>
auto Socket<IPV>::bindFirst(Lua::State* L) noexcept -> int {
  int bindRes = ::bind(fd, reinterpret_cast<sockaddr*>(addresses.data()), sizeof(SockAddrType));

  if(bindRes < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  return bindRes;
}

template<>
inline auto Socket<4>::pushIPAddress(Lua::State* L, const char* ip, uint16_t port, int idx) noexcept -> int {
  addresses[idx].sin_family = AF_INET;
  addresses[idx].sin_port   = port;
  int conversion = ::inet_pton(AF_INET, ip, &addresses[idx].sin_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<>
inline auto Socket<6>::pushIPAddress(Lua::State* L, const char* ip, uint16_t port, int idx) noexcept -> int {
  addresses[idx].sin6_family = AF_INET6;
  addresses[idx].sin6_port   = port;
  int conversion = ::inet_pton(AF_INET6, ip, &addresses[idx].sin6_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<int IPV>
auto Socket<IPV>::checkIPConversionResult(Lua::State* L, const char* ip, int result) noexcept -> int {
  if(result == 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "Invalid IP: %s", ip);
    return 2;
  } else if (result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, std::strerror(errno));
    return 2;
  }
  return 0;
}

template<int IPV>
auto Socket<IPV>::close(Lua::State* L) noexcept -> int {
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

