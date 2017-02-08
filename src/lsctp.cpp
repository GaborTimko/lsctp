#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"
#include "SctpServerSocket.hpp"

namespace {

template<int IPV, template<int> class SocketType>
inline auto UserDataToSocket(Lua::State* L, int idx) -> SocketType<IPV>* {
  //Note: it can raise an error
  return static_cast<SocketType<IPV>*>(Lua::Aux::CheckUData(L, idx, SocketType<IPV>::MetaTableName));
}

template<int IPV, template<int> class SocketType>
auto New(Lua::State* L) -> int {
  auto sock = Lua::NewUserData(L, sizeof(SocketType<IPV>));
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  try {
    new (sock) SocketType<IPV>();
  } catch(const std::system_error& ex) {
    Lua::PushNil(L);
    Lua::PushString(L, ex.what());
    return 2;
  }

  Lua::Aux::GetMetaTable(L, SocketType<IPV>::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPV, template<int> class SocketType>
using MemberFuncType = int (SocketType<IPV>::*)(Lua::State*);

template<int IPV,  template<int> class SocketType, MemberFuncType<IPV, SocketType> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV, SocketType>(L, 1);
  return (sock->*fn)(L);
}

template<int IPV>
using BaseMemberFuncType = int (Sctp::Socket<IPV>::*)(Lua::State*);

template<int IPV,  template<int> class SocketType, BaseMemberFuncType<IPV> fn>
auto CallBaseMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV, SocketType>(L, 1);
  return (sock->*fn)(L);
}

//template<int IPV>
//auto Accept(Lua::State* L) -> int {
//  auto sock = UserDataToSocket<IPV, Sctp::ServerSocket>(L, 1);
//  int newFD = sock->accept();
//  if(newFD < 0) {
//    Lua::PushBoolean(L, false);
//    Lua::PushFString(L, "accept() failed: %s", std::strerror(errno));
//    return 2;
//  }
//  auto connSock = Lua::NewUserData(L, sizeof(Sctp::Socket<IPV>));
//  if(connSock == nullptr) {
//    Lua::PushNil(L);
//    Lua::PushString(L, "Socket userdata allocation failed");
//    return 2;
//  }
//
//  new (connSock) Sctp::Socket<IPV>(newFD);
//
//  Lua::Aux::GetMetaTable(L, ChooseMetaTable<IPV, Sctp::Socket>());
//  Lua::SetMetaTable(L, -2);
//  return 1;
//}

const Lua::Aux::Reg SocketFuncs[] = {
  { nullptr, nullptr }
};

const Lua::Aux::Reg ServerSocketMeta4[] = {
  { "bind",   CallBaseMemberFunction<4, Sctp::ServerSocket, &Sctp::ServerSocket<4>::bind> },
  { "close",  CallBaseMemberFunction<4, Sctp::ServerSocket, &Sctp::ServerSocket<4>::close> },
  { "listen", CallMemberFunction<4, Sctp::ServerSocket,&Sctp::ServerSocket<4>::listen> },
//  { "accept", Accept<4> },
  { "__gc",   CallBaseMemberFunction<4, Sctp::ServerSocket, &Sctp::ServerSocket<4>::destroy> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ServerSocketMeta6[] = {
  { "bind",   CallBaseMemberFunction<6, Sctp::ServerSocket, &Sctp::ServerSocket<6>::bind> },
  { "close",  CallBaseMemberFunction<6, Sctp::ServerSocket, &Sctp::ServerSocket<6>::close> },
  { "listen", CallMemberFunction<6, Sctp::ServerSocket,&Sctp::ServerSocket<6>::listen> },
//  { "accept", Accept<6> },
  { "__gc",   CallBaseMemberFunction<6, Sctp::ServerSocket, &Sctp::ServerSocket<6>::destroy> },
  { nullptr, nullptr }
};

//const Lua::Aux::Reg ClientSocketMeta4[] = {
//  { "bind",   CallMemberFunction<4, &Sctp::ClientSocket<4>::bind> },
//  { "close",  CallMemberFunction<4, &Sctp::ClientSocket<4>::close> },
//  { "accept", Accept<4> },
//  { "__gc",   Destroy<4> },
//  { nullptr, nullptr }
//};
//
//const Lua::Aux::Reg ClientSocketMeta6[] = {
//  { "bind",   CallMemberFunction<6, &Sctp::ClientSocket<6>::bind> },
//  { "close",  CallMemberFunction<6, &Sctp::ClientSocket<6>::close> },
//  { "accept", Accept<6> },
//  { "__gc",   Destroy<6> },
//  { nullptr, nullptr }
//};

} //anonymous namespace

extern "C" int luaopen_sctp(Lua::State* L) {
  Lua::Aux::NewMetaTable(L, Sctp::ServerSocket<4>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ServerSocketMeta4, 0);

  Lua::Aux::NewMetaTable(L, Sctp::ServerSocket<6>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ServerSocketMeta6, 0);

  Lua::Aux::NewLib(L, SocketFuncs);

  Lua::Newtable(L);
  Lua::PushCFunction(L, New<4, Sctp::ServerSocket>);
  Lua::SetField(L, -2, "socket4");
  Lua::PushCFunction(L, New<6, Sctp::ServerSocket>);
  Lua::SetField(L, -2, "socket6");
  Lua::SetField(L, -2, "server");
  return 1;
}
