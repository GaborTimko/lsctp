#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"
#include "SctpServerSocket.hpp"
#include "SctpClientSocket.hpp"

namespace {

template<int IPV, template<int> class SocketType>
inline auto UserDataToSocket(Lua::State* L, int idx) -> SocketType<IPV>* {
  //Note: it can raise an error
  return Lua::Aux::CheckUData<SocketType<IPV>>(L, idx, SocketType<IPV>::MetaTableName);
}

template<int IPV, template<int> class SocketType>
auto New(Lua::State* L) -> int {
  auto sock = Lua::NewUserData<SocketType<IPV>>(L);
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  new (sock) SocketType<IPV>();
  if (not sock->create()) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "socket(): %s", std::strerror(errno));
    return 2;
  }

  Lua::Aux::GetMetaTable(L, SocketType<IPV>::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPV, template<int> class SocketType = Sctp::Socket::Base>
using MemberFuncType = int (SocketType<IPV>::*)(Lua::State*);

template<int IPV,  template<int> class SocketType, MemberFuncType<IPV, SocketType> fn>
inline auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV, SocketType>(L, 1);
  return (sock->*fn)(L);
}

template<int IPV,  template<int> class SocketType, MemberFuncType<IPV> fn>
inline auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPV, SocketType>(L, 1);
  auto basePtr = static_cast<Sctp::Socket::Base<IPV>*>(sock);
  return (basePtr->*fn)(L);
}

const Lua::Aux::Reg ServerSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::bind> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::close> },
  { "listen",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::listen> },
  { "accept",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::accept> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::setNonBlocking> },
  { "__gc",           CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::destroy> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ServerSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::bind> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::close> },
  { "listen",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::listen> },
  { "accept",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::accept> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::setNonBlocking> },
  { "__gc",           CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::destroy> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::bind> },
  { "connect",        CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::connect> },
  { "send",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::send> },
  { "recv",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::recv> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::close> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::setNonBlocking> },
  { "__gc",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::destroy> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::bind> },
  { "connect",        CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::connect> },
  { "send",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::send> },
  { "recv",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::recv> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::close> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::setNonBlocking> },
  { "__gc",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::destroy> },
  { nullptr, nullptr }
};

} //anonymous namespace

extern "C" int luaopen_sctp(Lua::State* L) {
  Lua::Aux::NewMetaTable(L, Sctp::Socket::Server<4>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ServerSocketMetaTable4, 0);

  Lua::Aux::NewMetaTable(L, Sctp::Socket::Server<6>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ServerSocketMetaTable6, 0);

  Lua::Aux::NewMetaTable(L, Sctp::Socket::Client<4>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ClientSocketMetaTable4, 0);

  Lua::Aux::NewMetaTable(L, Sctp::Socket::Client<6>::MetaTableName);
  Lua::PushValue(L, -1);
  Lua::SetField(L, -2, "__index");
  Lua::Aux::SetFuncs(L, ClientSocketMetaTable6, 0);

  const Lua::Aux::Reg SocketFuncs[] = { { nullptr, nullptr } };
  Lua::Aux::NewLib(L, SocketFuncs);

  Lua::Newtable(L);
  Lua::PushCFunction(L, New<4, Sctp::Socket::Server>);
  Lua::SetField(L, -2, "socket4");
  Lua::PushCFunction(L, New<6, Sctp::Socket::Server>);
  Lua::SetField(L, -2, "socket6");
  Lua::SetField(L, -2, "server");

  Lua::Newtable(L);
  Lua::PushCFunction(L, New<4, Sctp::Socket::Client>);
  Lua::SetField(L, -2, "socket4");
  Lua::PushCFunction(L, New<6, Sctp::Socket::Client>);
  Lua::SetField(L, -2, "socket6");
  Lua::SetField(L, -2, "client");

  return 1;
}
