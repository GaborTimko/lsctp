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
  Client();
  Client(int sock);
};

template<int IPV>
Client<IPV>::Client(int sock) : Base<IPV>(sock) {}

} //namespace Socket

} //namespace Sctp

#endif /* SCTPCLIENTSOCKET_HPP */

