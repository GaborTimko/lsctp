#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"

namespace Sctp {

namespace {

const char SocketMetaTableName4[] = "SctpSocket4";
const char SocketMetaTableName6[] = "SctpSocket6";

template<int IPV>
constexpr const char* ChooseMetaTable() {
  return IPV == 4 ? SocketMetaTableName4 : SocketMetaTableName6;
}

template<int IPV>
inline Socket<IPV>* UserDataToSocket(Lua::State* L, int idx) {
  static_assert(IPV == 4 or IPV == 6, "Invalid IP version");

  //Note: it can raise an error
  return static_cast<Socket<IPV>*>(Lua::Aux::CheckUData(L, idx, ChooseMetaTable<IPV>()));
}

template<int IPV>
int NewSocket(Lua::State* L) {
  static_assert(IPV == 4 or IPV == 6, "Invalid IP version");

  auto sock = Lua::NewUserData(L, sizeof(Socket<IPV>));
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  try {
    new (sock) Socket<IPV>();
  } catch(const std::system_error& ex) {
    Lua::PushNil(L);
    Lua::PushString(L, ex.what());
    return 2;
  }

  Lua::Aux::GetMetaTable(L, ChooseMetaTable<IPV>());
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPV>
int BindSocket(Lua::State* L) {
  auto sock = UserDataToSocket<IPV>(L, 1);
  return sock->bind(L);
}

template<int IPV>
int CloseSocket(Lua::State* L) {
  auto sock = UserDataToSocket<IPV>(L, 1);
  return sock->close(L);
}

const Lua::Aux::Reg SocketFuncs[] = {
  { "socket4", NewSocket<4> },
  { "socket6", NewSocket<6> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg SocketMeta4[] = {
  { "bind", BindSocket<4> },
  { "close", CloseSocket<4> },
  { "__gc", CloseSocket<4> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg SocketMeta6[] = {
  { "bind", BindSocket<6> },
  { "close", CloseSocket<6> },
  { "__gc", CloseSocket<6> },
  { nullptr, nullptr }
};

} //anonymous namespace

} //namespace Sctp

extern "C" int luaopen_sctp(Lua::State* L) {
  Lua::Aux::NewMetaTable(L, Sctp::SocketMetaTableName4);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, Sctp::SocketMeta4, 0);
  Lua::Aux::NewMetaTable(L, Sctp::SocketMetaTableName6);

  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, Sctp::SocketMeta6, 0);

  Lua::Aux::NewLib(L, Sctp::SocketFuncs);
  return 1;
}
