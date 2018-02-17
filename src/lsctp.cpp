#include <type_traits>

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

template<class SocketType>
auto UserDataToSocket(Lua::State* L, int idx) -> SocketType* {
  static_assert(Sctp::IsSctpSocket<SocketType>::value, "");
  return Lua::Aux::TestUData<SocketType>(L, idx, SocketType::MetaTableName);
}

template<class SocketType>
auto New(Lua::State* L) -> int {
  static_assert(Sctp::IsSctpSocket<SocketType>::value, "");
  auto sock = Lua::NewUserData<SocketType>(L);
  if(sock == nullptr) {
    Lua::PushNil(L);
    Lua::PushString(L, "Socket userdata allocation failed");
    return 2;
  }
  new (sock) SocketType();
  if (not sock->create()) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "socket(): %s", std::strerror(errno));
    return 2;
  }

  Lua::Aux::GetMetaTable(L, SocketType::MetaTableName);
  Lua::SetMetaTable(L, -2);
  return 1;
}

template<int IPVersion, template<int> class SocketType = Sctp::Socket::Base>
using MemberFuncType = int (SocketType<IPVersion>::*)(Lua::State*);

template<int IPVersion,  template<int> class SocketType, MemberFuncType<IPVersion, SocketType> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<SocketType<IPVersion>>(L, 1);
  if  (sock == nullptr) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "Can\'t call function, pointer is nil.");
    return 2;
  }
  return (sock->*fn)(L);
}

template<int IPVersion,  template<int> class SocketType, MemberFuncType<IPVersion> fn>
auto CallMemberFunction(Lua::State* L) -> int {
  auto sock = UserDataToSocket<SocketType<IPVersion>>(L, 1);
  if  (sock == nullptr) {
    Lua::PushBoolean(L, false);
    Lua::PushFString(L, "Can\'t call function, pointer is nil.");
    return 2;
  }
  auto basePtr = static_cast<Sctp::Socket::Base<IPVersion>*>(sock);
  return (basePtr->*fn)(L);
}

template<class SocketType>
auto DestroySocket(Lua::State* L) noexcept -> int {
  static_assert(Sctp::IsSctpSocket<SocketType>::value, "");
  auto sock = UserDataToSocket<SocketType>(L, 1);
  sock->SocketType::~SocketType();
  return 0;
}

//I haven't found a way yet to keep array of structures in the format below
//So for now, clang-format is off-limits
// clang-format off
const Lua::Aux::Reg ServerSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::bind> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::close> },
  { "listen",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::listen> },
  { "accept",         CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::accept> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Server, &Sctp::Socket::Server<4>::setNonBlocking> },
  { "__gc",           DestroySocket<Sctp::Socket::Server<4>> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ServerSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::bind> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::close> },
  { "listen",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::listen> },
  { "accept",         CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::accept> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Server, &Sctp::Socket::Server<6>::setNonBlocking> },
  { "__gc",           DestroySocket<Sctp::Socket::Server<6>> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable4[] = {
  { "bind",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::bind> },
  { "connect",        CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::connect> },
  { "send",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::send> },
  { "recv",           CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::recv> },
  { "close",          CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::close> },
  { "setnonblocking", CallMemberFunction<4, Sctp::Socket::Client, &Sctp::Socket::Client<4>::setNonBlocking> },
  { "__gc",           DestroySocket<Sctp::Socket::Client<4>> },
  { nullptr, nullptr }
};

const Lua::Aux::Reg ClientSocketMetaTable6[] = {
  { "bind",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::bind> },
  { "connect",        CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::connect> },
  { "send",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::send> },
  { "recv",           CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::recv> },
  { "close",          CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::close> },
  { "setnonblocking", CallMemberFunction<6, Sctp::Socket::Client, &Sctp::Socket::Client<6>::setNonBlocking> },
  { "__gc",           DestroySocket<Sctp::Socket::Client<6>> },
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
  Lua::PushCFunction(L, New<Sctp::Socket::Server<4>>);
  Lua::SetField(L, -2, "socket4");
  Lua::PushCFunction(L, New<Sctp::Socket::Server<6>>);
  Lua::SetField(L, -2, "socket6");
  Lua::SetField(L, -2, "server");

  Lua::Newtable(L);
  Lua::PushCFunction(L, New<Sctp::Socket::Client<4>>);
  Lua::SetField(L, -2, "socket4");
  Lua::PushCFunction(L, New<Sctp::Socket::Client<6>>);
  Lua::SetField(L, -2, "socket6");
  Lua::SetField(L, -2, "client");

  return 1;
}
