#include "SctpServerSocket.hpp"

namespace Sctp {

namespace Socket {

template<>
const char* Server<4>::MetaTableName = "Socket::ServerMeta4";

template<>
const char* Server<6>::MetaTableName = "Socket::ServerMeta6";

} //namespace Socket

} //namespace Sctp
