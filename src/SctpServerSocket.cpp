#include "SctpServerSocket.hpp"

namespace Sctp {

namespace Socket {

template<>
const char* Server<4>::MetaTableName = "ServerSocketMeta4";

template<>
const char* Server<6>::MetaTableName = "ServerSocketMeta6";

} //namespace Socket

} //namespace Sctp
