#ifndef SCTPCLIENTSOCKET_HPP
#define SCTPCLIENTSOCKET_HPP

#include "SctpSocket.hpp"

namespace Sctp {

namespace Socket {

template<int IPVersion>
class Client final : public Base<IPVersion> {
public:
  static const char* MetaTableName;
  static const std::size_t MaxRecvBufferSize = 5000;
private:
  char recvBuffer[MaxRecvBufferSize];
public:
  Client() : Base<IPVersion>() {}
  Client(int sock);
public:
  auto connect(Lua::State*) noexcept -> int;
  auto sendmsg(Lua::State*) noexcept -> int;
  auto recvmsg(Lua::State*) noexcept -> int;
};

template<int IPVersion>
Client<IPVersion>::Client(int sock) : Base<IPVersion>(sock) {}

template<int IPVersion>
auto Client<IPVersion>::connect(Lua::State* L) noexcept -> int {
  typename Base<IPVersion>::AddressArray peerAddresses;
  int loadAddrResult = this->loadAddresses(L, peerAddresses);
  if(loadAddrResult > 0) {
    return loadAddrResult;
  }

  if(::sctp_connectx(this->fd, reinterpret_cast<sockaddr*>(peerAddresses.data()), peerAddresses.size(), nullptr) < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "sctp_connectx: %s", std::strerror(errno));
    return 2;
  }

  Lua::PushBoolean(L, true);
  return 1;
}

template<int IPVersion>
auto Client<IPVersion>::sendmsg(Lua::State* L) noexcept -> int {
  std::size_t bufferLength;
  auto buffer = Lua::Aux::CheckLString(L, -1, bufferLength);

//  sctp_sndrcvinfo info;
//  std::memset(&info, 0, sizeof(sctp_sndrcvinfo));

  //TODO: Add support for choosing at least ppid, stream number and flags
  ssize_t numBytesSent = ::sctp_sendmsg(this->fd, buffer, bufferLength, nullptr, 0, 0, 0, 0, 0, 0);
  if(numBytesSent < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, (errno == EAGAIN ? "EAGAIN" : "sctp_sendmsg: %s"), std::strerror(errno));
    return 2;
  }
  Lua::PushInteger(L, numBytesSent);
  return 1;
}

template<int IPVersion>
auto Client<IPVersion>::recvmsg(Lua::State* L) noexcept -> int {
//  sctp_sndrcvinfo info;
//  std::memset(&info, 0, sizeof(sctp_sndrcvinfo));

  //TODO: Add support for filling sctp_sndrcvinfo and flags
  std::memset(&recvBuffer, 0, sizeof(recvBuffer));
  ssize_t numBytesReceived = ::sctp_recvmsg(this->fd, recvBuffer, MaxRecvBufferSize, nullptr, nullptr, nullptr, nullptr);
  if(numBytesReceived < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, (errno == EAGAIN or errno == EWOULDBLOCK ? "EAGAIN/EWOULDBLOCK" : "sctp_recvmsg: %s"), std::strerror(errno));
    return 2;
  }
  Lua::PushInteger(L, numBytesReceived);
  Lua::PushLString(L, recvBuffer, numBytesReceived);
  return 2;
}

} //namespace Socket

} //namespace Sctp

#endif /* SCTPCLIENTSOCKET_HPP */

