#include "SctpServerSocket.hpp"

namespace Sctp {

template<>
const char* ServerSocket<4>::MetaTableName = "ServerSocketMeta4";

template<>
const char* ServerSocket<6>::MetaTableName = "ServerSocketMeta6";

}
