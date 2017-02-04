#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"

namespace Sctp {

const char SocketMetaTableName[] = "SctpSocket";

inline Socket* UserDataToSocket(Lua::State* L, int idx) {
  return static_cast<Socket*>(Lua::Aux::CheckUData(L, idx, SocketMetaTableName));
}

int NewSocket(Lua::State* L) {
  int ipVersion = Lua::ToInteger(L, -1);

  auto sock = Lua::NewUserData(L, sizeof(Socket));
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  try {
    new (sock) Socket(ipVersion);
  } catch(const std::system_error& ex) {
    Lua::PushNil(L);
    Lua::PushString(L, ex.what());
    return 2;
  }

  Lua::Aux::GetMetaTable(L, SocketMetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

const Lua::Aux::Reg SocketFuncs[] = {
  { "socket", NewSocket },
  { nullptr, nullptr }
};

const Lua::Aux::Reg SocketMeta[] = {
  { nullptr, nullptr }
};

}

extern "C" int luaopen_sctp(Lua::State* L) {
  Lua::Aux::NewMetaTable(L, Sctp::SocketMetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, Sctp::SocketMeta, 0);
  Lua::Aux::NewLib(L, Sctp::SocketFuncs);
  return 1;
}
