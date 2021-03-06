#ifndef LSSOCKET_HPP
#define LSSOCKET_HPP

#include <vector>
#include <cstring>
#include <cerrno>
#include <type_traits>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "Lua/Lua.hpp"

namespace Sctp {

namespace Socket {

template<int IPVersion>
class Base {
  static_assert(IPVersion == 4 or IPVersion == 6, "");
public:
  using SockAddrType = std::conditional_t<IPVersion == 4, sockaddr_in, sockaddr_in6>;
  using AddressArray = std::vector<SockAddrType>;
  static constexpr int IPv = IPVersion;
protected:
  int fd;
  bool haveBoundAddresses;
  AddressArray boundAddresses;
public:
  Base() noexcept;
  Base(int sock) noexcept;
  ~Base();
public:
  auto create() noexcept -> bool;
  auto bind(Lua::State*) noexcept -> int;
  auto close(Lua::State*) noexcept -> int;
  auto setNonBlocking(Lua::State*) noexcept -> int;
protected:
  auto loadAddresses(Lua::State*, AddressArray&) noexcept -> int;
private:
  auto bindFirst(Lua::State*) noexcept -> int;
  auto pushIPAddress(Lua::State*, AddressArray&, const char* ip, uint16_t port, int idx) noexcept -> int;
  auto checkIPConversionResult(Lua::State*, const char* ip, int result) noexcept -> int;
};

template<int IPVersion>
Base<IPVersion>::Base() noexcept : haveBoundAddresses(false) {}

template<int IPVersion>
auto Base<IPVersion>::create() noexcept -> bool {
  fd = ::socket(IPVersion == 4 ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_SCTP);
  if(fd == -1) {
    return false;
  }
  int True = 1;
  setsockopt(fd, IPPROTO_SCTP, SO_REUSEADDR, &True, sizeof(int));
  return true;
}

template<int IPVersion>
Base<IPVersion>::Base(int sock) noexcept : fd(sock), haveBoundAddresses(false) {}

template<int IPVersion>
Base<IPVersion>::~Base() {
  ::close(fd);
}

template<int IPVersion>
 auto Base<IPVersion>::setNonBlocking(Lua::State* L) noexcept -> int {
  int flags = fcntl(fd, F_GETFL);
  if(flags < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "fcntl(get): %s", std::strerror(errno));
    return 2;
  }
  flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  if(flags < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "fcntl(set): %s", std::strerror(errno));
    return 2;
  }
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPVersion>
auto Base<IPVersion>::bind(Lua::State* L) noexcept -> int {
  int loadAddrResult = loadAddresses(L, boundAddresses);
  std::size_t addrCount = boundAddresses.size();
  if(loadAddrResult > 0) {
    return loadAddrResult;
  }

  int retVal = bindFirst(L);
  if(retVal > 0) {
    return retVal;
  }

  if(addrCount > 1) {
    if(::sctp_bindx(fd, reinterpret_cast<sockaddr*>(boundAddresses.data() + 1), addrCount - 1, SCTP_BINDX_ADD_ADDR) < 0) {
      Lua::PushBoolean(L, false);
      Lua::PushFString(L, "sctp_bindx: %s", std::strerror(errno));
      return 2;
    }
  }

  haveBoundAddresses = true;
  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPVersion>
auto Base<IPVersion>::loadAddresses(Lua::State* L, AddressArray& addrs) noexcept -> int {
  uint16_t port = htons(Lua::ToInteger(L, 2));
  int stackSize = Lua::GetTop(L);
  std::size_t addrCount = stackSize - 2;
  if(addrCount < 1) {
    Lua::PushBoolean(L, false);
    Lua::PushString(L, "No addresses were given");
    return 2;
  }
  addrs.clear();
  addrs.resize(addrCount);
  std::memset(addrs.data(), 0, sizeof(SockAddrType) * addrCount);
  for(int i = 3; i <= stackSize; i++) {
    int idx = i - 3;
    auto addrI = Lua::ToString(L, i);
    int pushResult = pushIPAddress(L, addrs, addrI, port, idx);
    if(pushResult > 0) {
      return pushResult;
    }
  }
  return 0;
}

template<int IPVersion>
auto Base<IPVersion>::bindFirst(Lua::State* L) noexcept -> int {
  int bindRes = ::bind(fd, reinterpret_cast<sockaddr*>(boundAddresses.data()), sizeof(SockAddrType));

  if(bindRes < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "bind: %s",  std::strerror(errno));
    return 2;
  }
  return bindRes;
}

template<>
inline auto Base<4>::pushIPAddress(Lua::State* L, AddressArray& addrs, const char* ip, uint16_t port, int idx) noexcept -> int {
  addrs[idx].sin_family = AF_INET;
  addrs[idx].sin_port   = port;
  int conversion = ::inet_pton(AF_INET, ip, &addrs[idx].sin_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<>
inline auto Base<6>::pushIPAddress(Lua::State* L, AddressArray& addrs, const char* ip, uint16_t port, int idx) noexcept -> int {
  addrs[idx].sin6_family = AF_INET6;
  addrs[idx].sin6_port   = port;
  int conversion = ::inet_pton(AF_INET6, ip, &addrs[idx].sin6_addr);
  return checkIPConversionResult(L, ip, conversion);
}

template<int IPVersion>
auto Base<IPVersion>::checkIPConversionResult(Lua::State* L, const char* ip, int result) noexcept -> int {
  if(result == 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "inet_pton: invalid IP: %s", ip);
    return 2;
  } else if (result < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "inet_pton: %s", std::strerror(errno));
    return 2;
  }
  return 0;
}

template<int IPVersion>
auto Base<IPVersion>::close(Lua::State* L) noexcept -> int {
  if(fd > -1 and ::close(fd) < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "close: %s", std::strerror(errno));
    fd = -1;
    return 2;
  }
  fd = -1;
  boundAddresses.clear();
  Lua::PushBoolean(L, true);
  return 1;
}

} //namespace Socket

template<class Type>
struct IsSctpSocket : public std::integral_constant<bool,
                                                    std::is_same<Type, Socket::Base<4>>::value or
                                                    std::is_same<Type, Socket::Base<6>>::value or
                                                    std::is_base_of<Socket::Base<4>, Type>::value or
                                                    std::is_base_of<Socket::Base<6>, Type>::value> {
};

} //namespace Sctp

#endif /* LSSOCKET_HPP */

