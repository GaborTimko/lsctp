#include "Lua/Lua.hpp"
#include "SctpSocket.hpp"
#include "SctpServerSocket.hpp"
#include "SctpClientSocket.hpp"

namespace Sctp {

namespace Socket {

template<> const char* Server<4>::MetaTableName = "ServerSocketMeta4";

template<> const char* Server<6>::MetaTableName = "ServerSocketMeta6";

template<> const char* Client<4>::MetaTableName = "ClientSocketMeta4";

template<> const char* Client<6>::MetaTableName = "ClientSocketMeta6";

} //namespace Socket

} //namespace Sctp

namespace {

template<int IPVersion, template<int> class SocketType>
auto UserDataToSocket(Lua::State* L, int idx) -> SocketType<IPVersion>* {
  return Lua::Aux::TestUData<SocketType<IPVersion>>(L, idx, SocketType<IPVersion>::MetaTableName);
}

template<int IPVersion, template<int> class SocketType>
auto New(Lua::State* L) -> int {
  auto sock = Lua::NewUserData<SocketType<IPVersion>>(L);
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  new (sock) SocketType<IPVersion>();
  if (not sock->create()) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "socket(): %s", std::strerror(errno));
    return 2;
  }

  Lua::Aux::GetMetaTable(L, SocketType<IPVersion>::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPVersion, template<int> class SocketType = Sctp::Socket::Base>
using MemberFuncType = int (SocketType<IPVersion>::*)(Lua::State*);

template<int IPVersion,  template<int> class SocketType, MemberFuncType<IPVersion, SocketType> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPVersion, SocketType>(L, 1);
  if  (sock == nullptr) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "Can\'t call function, pointer is nil.");
    return 2;
  }
  return (sock->*fn)(L);
}

template<int IPVersion,  template<int> class SocketType, MemberFuncType<IPVersion> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<IPVersion, SocketType>(L, 1);
  if  (sock == nullptr) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "Can\'t call function, pointer is nil.");
    return 2;
  }
  auto basePtr = static_cast<Sctp::Socket::Base<IPVersion>*>(sock);
  return (basePtr->*fn)(L);
}

template<int IPVersion, template<int> class SocketType>
auto DestroySocket(Lua::State* L) noexcept -> int {
  auto sock = UserDataToSocket<IPVersion, SocketType>(L, 1);
  sock->SocketType<IPVersion>::~SocketType();
  return 0;
}

//I haven't find a way yet to keep array of structures in the format below
//So for now, clang-format is off-limits
// clang-format off
const Lua::Aux::Reg ServerSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::bind> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::close> },
  { "listen",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::listen> },
  { "accept",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::accept> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::setNonBlocking> },
  { "__gc",           DestroySocket<4, Sctp::Socket::Server> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ServerSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::bind> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::close> },
  { "listen",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::listen> },
  { "accept",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::accept> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::setNonBlocking> },
  { "__gc",           DestroySocket<6, Sctp::Socket::Server> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::bind> },
  { "connect",        CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::connect> },
  { "send",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::send> },
  { "recv",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::recv> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::close> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::setNonBlocking> },
  { "__gc",           DestroySocket<4, Sctp::Socket::Client> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::bind> },
  { "connect",        CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::connect> },
  { "send",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::send> },
  { "recv",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::recv> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::close> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::setNonBlocking> },
  { "__gc",           DestroySocket<6, Sctp::Socket::Client> },
  { nullptr, nullptr }
};
// clang-format on

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
