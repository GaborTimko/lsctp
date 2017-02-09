#include "SctpClientSocket.hpp"

namespace Sctp {

namespace Socket {

template<>
const char* Client<4>::MetaTableName = "ClientSocketMeta4";

template<>
const char* Client<6>::MetaTableName = "ClientSocketMeta6";

} //namespace Socket

} //namespace Sctp
