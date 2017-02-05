#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"

namespace {

const char SocketMetaTableName4[] = "SctpSocket4";
const char SocketMetaTableName6[] = "SctpSocket6";

template<int IPV>
constexpr auto ChooseMetaTable() -> const char* {
  return IPV == 4 ? SocketMetaTableName4 : SocketMetaTableName6;
}

template<int IPV>
inline auto UserDataToSocket(Lua::State* L, int idx) -> Sctp::Socket<IPV>* {
  //Note: it can raise an error
  return static_cast<Sctp::Socket<IPV>*>(Lua::Aux::CheckUData(L, idx, ChooseMetaTable<IPV>()));
}

template<int IPV>
auto New(Lua::State* L) -> int {
  auto sock = Lua::NewUserData(L, sizeof(Sctp::Socket<IPV>));
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  try {
    new (sock) Sctp::Socket<IPV>();
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
using MemberFuncType = int (Sctp::Socket<IPV>::*)(Lua::State*);

template<int IPV,  MemberFuncType<IPV> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV>(L, 1);
  return (sock->*fn)(L);
}

template<int IPV>
auto Accept(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV>(L, 1);
  int newFD = sock->accept();
  if(newFD < 0) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "accept() failed: %s", std::strerror(errno));
    return 2;
  }
  auto connSock = Lua::NewUserData(L, sizeof(Sctp::Socket<IPV>));
  if(connSock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  
  new (connSock) Sctp::Socket<IPV>(newFD);

  Lua::Aux::GetMetaTable(L, ChooseMetaTable<IPV>());
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPV>
auto Destroy(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV>(L, 1);
  sock->~Socket();
  Lua::PushBoolean(L, true);
  return 1;
}

const Lua::Aux::Reg SocketFuncs[] = {
  { "socket4", New<4> },
  { "socket6", New<6> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg SocketMeta4[] = {
  { "bind",   CallMemberFunction<4, &Sctp::Socket<4>::bind> },
  { "listen", CallMemberFunction<4, &Sctp::Socket<4>::listen> },
  { "close",  CallMemberFunction<4, &Sctp::Socket<4>::close> },
  { "accept", Accept<4> },
  { "__gc",   Destroy<4> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg SocketMeta6[] = {
  { "bind",   CallMemberFunction<6, &Sctp::Socket<6>::bind> },
  { "listen", CallMemberFunction<6, &Sctp::Socket<6>::listen> },
  { "close",  CallMemberFunction<6, &Sctp::Socket<6>::close> },
  { "accept", Accept<6> },
  { "__gc",   Destroy<6> },
  { nullptr, nullptr }
};

} //anonymous namespace

extern "C" int luaopen_sctp(Lua::State* L) {
  Lua::Aux::NewMetaTable(L, SocketMetaTableName4);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, SocketMeta4, 0);

  Lua::Aux::NewMetaTable(L, SocketMetaTableName6);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, SocketMeta6, 0);

  Lua::Aux::NewLib(L, SocketFuncs);
  return 1;
}
