#ifndef LUA_HPP
#define LUA_HPP

#include <type_traits>
#include <cstddef>
#include <cstdarg>
#include <cstdio>

namespace detail {

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

constexpr bool LoadModeChecker(const char* c) {
  return c == nullptr or
        (c[0] == 't' and c[1] == '\0') or
        (c[0] == 'b' and c[1] == '\0') or
        (c[0] == 'b' and c[1] == 't');
}

} //Namespace detail

namespace Lua {

namespace Conf {

constexpr auto ExtraSpace = LUA_EXTRASPACE;
#undef LUA_EXTRASPACE

} //namespace Conf

constexpr auto VersionMajor   = LUA_VERSION_MAJOR;
constexpr auto VersionMinor   = LUA_VERSION_MINOR;
constexpr auto VersionNumber  = LUA_VERSION_NUM;
constexpr auto VersionRelease = LUA_VERSION_RELEASE;

//Version renamed to prevent name clash with the function Version
constexpr auto VersionStr = LUA_VERSION;
constexpr auto Release    = LUA_RELEASE;
constexpr auto CopyRight  = LUA_COPYRIGHT;
constexpr auto Authors    = LUA_AUTHORS;

#undef LUA_VERSION_MAJOR
#undef LUA_VERSION_MINOR
#undef LUA_VERSION_NUM
#undef LUA_VERSION_RELEASE
#undef LUA_VERSION
#undef LUA_RELEASE
#undef LUA_COPYRIGHT
#undef LUA_AUTHORS

constexpr auto Signature      = LUA_SIGNATURE;
constexpr auto MultiReturn    = LUA_MULTRET;
constexpr auto RegistryIndex  = LUA_REGISTRYINDEX;

template<class IdxType>
constexpr auto UpvalueIndex(const IdxType i) -> decltype(lua_upvalueindex(-1)) {
  static_assert(std::is_arithmetic<IdxType>::value, "");
  return RegistryIndex - i;
}

#undef LUA_SIGNATURE
#undef LUA_MULTRET
#undef LUA_REGISTRYINDEX
#undef lua_upvalueindex

enum class Statuses {
  OK              = LUA_OK,
  Yield           = LUA_YIELD,
  RuntimeError    = LUA_ERRRUN,
  SyntaxError     = LUA_ERRSYNTAX,
  MemAllocError   = LUA_ERRMEM,
  GCMethaMError   = LUA_ERRGCMM,
  MsgHandlerError = LUA_ERRERR,
  FileError       = LUA_ERRFILE
};

#undef LUA_OK
#undef LUA_YIELD
#undef LUA_ERRRUN
#undef LUA_ERRSYNTAX
#undef LUA_ERRMEM
#undef LUA_ERRGCMM
#undef LUA_ERRERR
#undef LUA_ERRFILE

using State = detail::lua_State;

enum class Types {
  None          = LUA_TNONE,
  Nil           = LUA_TNIL,
  Boolean       = LUA_TBOOLEAN,
  LightUserData = LUA_TLIGHTUSERDATA,
  Number        = LUA_TNUMBER,
  String        = LUA_TSTRING,
  Table         = LUA_TTABLE,
  Function      = LUA_TFUNCTION,
  UserData      = LUA_TUSERDATA,
  Thread        = LUA_TTHREAD,
  NumTags       = LUA_NUMTAGS
};

#undef LUA_TNONE
#undef LUA_TNIL
#undef LUA_TBOOLEAN
#undef LUA_TLIGHTUSERDATA
#undef LUA_TNUMBER
#undef LUA_TSTRING
#undef LUA_TTABLE
#undef LUA_TFUNCTION
#undef LUA_TUSERDATA
#undef LUA_TTHREAD
#undef LUA_NUMTAGS


template<class ConstType>
constexpr ConstType MinStack = ConstType(LUA_MINSTACK);

#undef LUA_MINSTACK


enum class RidX {
  MainThread  = LUA_RIDX_MAINTHREAD,
  Globals     = LUA_RIDX_GLOBALS,
  Last        = RidX::Globals
};

#undef LUA_RIDX_MAINTHREAD
#undef LUA_RIDX_GLOBALS

using Number    = detail::lua_Number;
using Integer   = detail::lua_Integer;
using Unsigned  = detail::lua_Unsigned;
using KContext  = detail::lua_KContext;

using CFunction = detail::lua_CFunction;
using KFunction = detail::lua_KFunction;
using Reader    = detail::lua_Reader;
using Writer    = detail::lua_Writer;
using Alloc     = detail::lua_Alloc;

//auto Ident = detail::lua_ident;

inline State* NewState(Alloc f, void* ud) {
  return detail::lua_newstate(f, ud);
}
inline void Close(State* L) {
  detail::lua_close(L);
}
inline State* NewThread(State* L) {
  return detail::lua_newthread(L);
}
inline CFunction AtPanic(State* L, CFunction panicf) {
  return detail::lua_atpanic(L, panicf);
}
inline const Number* Version(State* L) {
  return detail::lua_version(L);
}
inline bool IsNumber(State* L, int idx) {
  return static_cast<bool>(detail::lua_isnumber(L, idx));
}


inline int AbsIndex(State* L, int idx) {
  return detail::lua_absindex(L, idx);
}
inline int GetTop(State* L) {
  return detail::lua_gettop(L);
}
inline void SetTop(State* L, int idx) {
  detail::lua_settop(L, idx);
}
inline void PushValue(State* L, int idx) {
  detail::lua_pushvalue(L, idx);
}
inline void Rotate(State* L, int idx, int n) {
  detail::lua_rotate(L, idx, n);
}
inline void Copy(State* L, int fromIdx, int toIdx) {
  detail::lua_copy(L, fromIdx, toIdx);
}
inline bool CheckStack(State* L, int n) {
  return static_cast<bool>(detail::lua_checkstack(L, n));
}
inline void XMove(State* from, State* to, int n) {
  detail::lua_xmove(from, to, n);
}


inline bool IsString(State* L, int idx) {
  return static_cast<bool>(detail::lua_isstring(L, idx));
}
inline bool IsCFunction(State* L, int idx) {
  return static_cast<bool>(detail::lua_iscfunction(L, idx));
}
inline bool IsInteger(State* L, int idx) {
  return static_cast<bool>(detail::lua_isinteger(L, idx));
}
inline bool IsUserData(State* L, int idx) {
  return static_cast<bool>(detail::lua_isuserdata(L, idx));
}
inline Types Type(State* L, int idx) {
  return static_cast<Types>(detail::lua_type(L, idx));
}
inline const char* TypeName(State* L, Types tp) {
  return detail::lua_typename(L, static_cast<int>(tp));
}
inline Number ToNumberX(State* L, int idx, int* isNum) {
  return detail::lua_tonumberx(L, idx, isNum);
}
inline Integer ToIntegerX(State* L, int idx, int* isNum) {
  return detail::lua_tointegerx(L, idx, isNum);
}
inline bool ToBoolean(State* L, int idx) {
  return static_cast<bool>(detail::lua_toboolean(L, idx));
}
inline const char* ToLString(State* L, int idx, std::size_t* len) {
  return detail::lua_tolstring(L, idx, len);
}
inline std::size_t RawLen(State* L, int idx) {
  return detail::lua_rawlen(L, idx);
}
inline CFunction ToCFunction(State* L, int idx) {
  return detail::lua_tocfunction(L, idx);
}
inline void* ToUserData(State* L, int idx) {
  return detail::lua_touserdata(L, idx);
}
inline State* ToThread(State* L, int idx) {
  return detail::lua_tothread(L, idx);
}
inline const void* ToPointer(State* L, int idx) {
  return detail::lua_topointer(L, idx);
}


//TODO::Consider later if it's possible to do this all
//arithmetic, with operator overloading
enum class ArithmeticOperators {
  Add           = LUA_OPADD,
  Sub           = LUA_OPSUB,
  Mul           = LUA_OPMUL,
  Mod           = LUA_OPMOD,
  Pow           = LUA_OPPOW,
  FloatDiv      = LUA_OPDIV,
  FloorDiv      = LUA_OPIDIV,
  BitwiseAnd    = LUA_OPBAND,
  BitwiseOr     = LUA_OPBOR,
  BitwiseXor    = LUA_OPBXOR,
  ShiftLeft     = LUA_OPSHL,
  ShiftRight    = LUA_OPSHR,
  UnaryNegation = LUA_OPUNM,
  BitwiseNot    = LUA_OPBNOT,
};

enum class RelationalOperators {
  Equals        = LUA_OPEQ,
  LessThan      = LUA_OPLT,
  LessOrEqual   = LUA_OPLE
};

#undef LUA_OPADD
#undef LUA_OPSUB
#undef LUA_OPMUL
#undef LUA_OPMOD
#undef LUA_OPPOW
#undef LUA_OPDIV
#undef LUA_OPIDIV
#undef LUA_OPBAND
#undef LUA_OPBOR
#undef LUA_OPBXOR
#undef LUA_OPSHL
#undef LUA_OPSHR
#undef LUA_OPUNM
#undef LUA_OPBNOT
#undef LUA_OPEQ
#undef LUA_OPLT
#undef LUA_OPLE

inline void Arith(State* L, ArithmeticOperators op) {
  detail::lua_arith(L, static_cast<int>(op));
}
//TODO: should these 2 functions return bool?
inline bool RawEqual(State* L, int idx1, int idx2) {
  return static_cast<bool>(detail::lua_rawequal(L, idx1, idx2));
}
inline bool Compare(State* L, int idx1, int idx2, RelationalOperators op) {
  return static_cast<bool>(detail::lua_compare(L, idx1, idx2, static_cast<int>(op)));
}


//TODO: check for possibility of function overloading (+type_traits)
inline void PushNil(State* L) {
  detail::lua_pushnil(L);
}
inline void PushNumber(State* L, Number n) {
  detail::lua_pushnumber(L, n);
}
inline void PushInteger(State* L, Integer n) {
  detail::lua_pushinteger(L, n);
}
inline const char* PushLString(State* L, const char* s, std::size_t len) {
  return detail::lua_pushlstring(L, s, len);
}
inline const char* PushString(State* L, const char* s) {
  return detail::lua_pushstring(L, s);
}
inline const char* PushVFString(State* L, const char* fmt, va_list argp) {
  return detail::lua_pushvfstring(L, fmt, argp);
}
template<class ...VarArgs>
inline const char* PushFString(State* L, const char* fmt, VarArgs&&... args) {
  return detail::lua_pushfstring(L, fmt, args...);
}
inline void PushCClosure(State* L, CFunction fn, int n) {
  detail::lua_pushcclosure(L, fn, n);
}
inline void PushBoolean(State* L, bool b) {
  detail::lua_pushboolean(L, static_cast<int>(b));
}
inline void PushLightUserData(State* L, void* p) {
  detail::lua_pushlightuserdata(L, p);
}
inline int PushThread(State* L) {
  return detail::lua_pushthread(L);
}


//TODO: function overloading
inline Types GetGlobal(State* L, const char* name) {
  return static_cast<Types>(detail::lua_getglobal(L, name));
}
inline Types GetTable(State* L, int idx) {
  return static_cast<Types>(detail::lua_gettable(L, idx));
}
inline Types GetField(State* L, int idx, const char* k) {
  return static_cast<Types>(detail::lua_getfield(L, idx, k));
}
inline Types Geti(State* L, int idx, Integer n) {
  return static_cast<Types>(detail::lua_geti(L, idx, n));
}
inline Types RawGet(State* L, int idx) {
  return static_cast<Types>(detail::lua_rawget(L, idx));
}
inline Types RawGet(State* L, int idx, Integer n) {
  return static_cast<Types>(detail::lua_rawgeti(L, idx, n));
}
inline Types RawGet(State* L, int idx, const void* p) {
  return static_cast<Types>(detail::lua_rawgetp(L, idx, p));
}
inline void CreateTable(State* L, int narr, int nrec) {
  detail::lua_createtable(L, narr, nrec);
}
inline void* NewUserData(State* L, std::size_t sz) {
  return detail::lua_newuserdata(L, sz);
}
inline Types GetMetaTable(State* L, int objindex) {
  return static_cast<Types>(detail::lua_getmetatable(L, objindex));
}
inline Types GetUserValue(State* L, int idx) {
  return static_cast<Types>(detail::lua_getuservalue(L, idx));
}

inline void SetGlobal(State* L, const char* name) {
  detail::lua_setglobal(L, name);
}
inline void SetTable(State* L, int idx) {
  detail::lua_settable(L, idx);
}
inline void SetField(State* L, int idx, const char* k) {
  detail::lua_setfield(L, idx, k);
}
inline void Seti(State* L, int idx, Integer n) {
  detail::lua_seti(L, idx, n);
}
inline void RawSet(State* L, int idx) {
  detail::lua_rawset(L, idx);
}
inline void RawSet(State* L, int idx, Integer n) {
  detail::lua_rawseti(L, idx, n);
}
inline void RawSet(State* L, int idx, const void* p) {
  detail::lua_rawsetp(L, idx, p);
}
inline int SetMetaTable(State* L, int objindex) {
  return detail::lua_setmetatable(L, objindex);
}
inline void SetUserValue(State* L, int idx) {
  detail::lua_setuservalue(L, idx);
}



inline void CallK(State* L, int nargs, int nresults, KContext ctx, KFunction k) {
  detail::lua_callk(L, nargs, nresults, ctx, k);
}
inline void Call(State* L, int nargs, int nresults) {
  CallK(L, nargs, nresults, 0, nullptr);
}

inline Statuses PCallK(State* L, int nargs, int nresults, int errfunc, KContext ctx, KFunction k) {
  return static_cast <Statuses>(detail::lua_pcallk(L, nargs, nresults, errfunc, ctx, k));
}
inline Statuses PCall(State* L,int nargs, int nresults, int errfunc) {
  return static_cast<Statuses>(PCallK(L, nargs, nresults, errfunc, 0, nullptr));
}

template<size_t N>
inline Statuses Load(State* L, Reader reader, void* dt, const char* chunkname, const char (&mode)[N]) {
  static_assert(::detail::LoadModeChecker(mode), R"(Load mode can only be "b", "t", "bt" or null pointer)");
  return static_cast<Statuses>(detail::lua_load(L, reader, dt, chunkname, mode));
}

inline Statuses Load(State* L, Reader reader, void* dt, const char* chunkname, const char* mode) {
  return static_cast<Statuses>(detail::lua_load(L, reader, dt, chunkname, mode));
}
inline int Dump(State* L, Writer writer, void* data, int strip) {
  return detail::lua_dump(L, writer, data, strip);
}


inline int YieldK(State* L, int nresults, KContext ctx, KFunction k) {
  return detail::lua_yieldk(L, nresults, ctx, k);
}
inline Statuses Resume(State* L, State* from, int narg) {
  return static_cast<Statuses>(detail::lua_resume(L, from, narg));
}
inline Statuses Status(State* L) {
  return static_cast<Statuses>(detail::lua_status(L));
}
inline bool IsYieldable(State* L) {
  return static_cast<bool>(detail::lua_isyieldable(L));
}
inline int Yield(State* L,int nresults) {
  return YieldK(L, nresults, 0, nullptr);
}


enum class GarbageCollectionOpts {
  Stop        = LUA_GCSTOP,
  Restart     = LUA_GCRESTART,
  Collect     = LUA_GCCOLLECT,
  Count       = LUA_GCCOUNT,
  CountB      = LUA_GCCOUNTB,
  Step        = LUA_GCSTEP,
  SetPause    = LUA_GCSETPAUSE,
  SetStupMul  = LUA_GCSETSTEPMUL,
  IsRunning   = LUA_GCISRUNNING
};

#undef LUA_GCSTOP
#undef LUA_GCRESTART
#undef LUA_GCCOLLECT
#undef LUA_GCCOUNT
#undef LUA_GCCOUNTB
#undef LUA_GCSTEP
#undef LUA_GCSETPAUSE
#undef LUA_GCSETSTEPMUL
#undef LUA_GCISRUNNING

inline int GC(State* L, GarbageCollectionOpts what, int data) {
  return detail::lua_gc(L, static_cast<int>(what), data);
}


inline int Error(State* L) {
  return detail::lua_error(L);
}
inline int Next(State* L, int idx) {
  return detail::lua_next(L, idx);
}
inline void Concat(State* L, int n) {
  detail::lua_concat(L, n);
}
inline void Len(State* L, int idx) {
  detail::lua_len(L, idx);
}
inline std::size_t StringToNumber(State* L, const char* s) {
  return detail::lua_stringtonumber(L, s);
}
inline Alloc GetAllocF(State* L, void** ud) {
  return detail::lua_getallocf(L, ud);
}
inline void SetAllocF(State* L, Alloc f, void* ud) {
  detail::lua_setallocf(L, f, ud);
}


inline void* GetExtraSpace(State* L) {
  return ((void *)((char *)(L) - Conf::ExtraSpace));
}
inline Number ToNumber(State* L, int idx){
  return ToNumberX(L, idx, nullptr);
}
inline Integer ToInteger(State* L,int idx) {
  return ToIntegerX(L, idx, nullptr);
}
inline void Pop(State* L, int idx) {
  SetTop(L, -idx-1);
}
inline void Newtable(State* L) {
  CreateTable(L, 0, 0);
}
inline void PushCFunction(State* L, CFunction fn) {
  PushCClosure(L, fn, 0);
}

inline void Register(State* L, const char* name, CFunction fn) {
  PushCFunction(L, fn);
  SetGlobal(L, name);
}
inline bool IsFunction(State* L, int idx) {
  return Type(L, idx) == Types::Function;
}
inline bool IsTable(State* L, int idx) {
  return Type(L, idx) == Types::Table;
}
inline bool IsLightUserData(State* L, int idx) {
  return Type(L, idx) == Types::LightUserData;
}
inline bool IsNil(State* L, int idx) {
  return Type(L, idx) == Types::Nil;
}
inline bool IsBoolean(State* L, int idx) {
  return Type(L, idx) == Types::Boolean;
}
inline bool IsThread(State* L, int idx) {
  return Type(L, idx) == Types::Thread;
}
inline bool IsNone(State* L, int idx) {
  return Type(L, idx) == Types::None;
}
inline bool IsNoneOrNil(State* L, int idx) {
  return static_cast<int>(Type(L, idx)) <= 0;
}
inline const char* PushLiteral(State* L, const char* s) {
  return PushString(L, s);
}
//TODO: Check RawGet's input params and convert macro to const
inline void PushGlobalTable(State* L) {
  RawGet(L, RegistryIndex, static_cast<int>(RidX::Globals));
}
inline const char* ToString(State* L, int idx) {
  return ToLString(L, idx, nullptr);
}
inline void Insert(State* L, int idx) {
  Rotate(L, idx, 1);
}
inline void Remove(State* L, int idx) {
  Rotate(L, idx, (-1));
  Pop(L, 1);
}
inline void Replace(State* L, int idx) {
  Copy(L, -1, idx);
  Pop(L, 1);
}

#undef lua_getextraspace
#undef lua_tonumber
#undef lua_tointeger
#undef lua_pop
#undef lua_newtable
#undef lua_register
#undef lua_pushcfunction
#undef lua_isfunction
#undef lua_istable
#undef lua_islightuserdata
#undef lua_isnil
#undef lua_isboolean
#undef lua_isthread
#undef lua_isnone
#undef lua_isnoneornil
#undef lua_pushliteral
#undef lua_pushglobaltable
#undef lua_tostring
#undef lua_insert
#undef lua_remove
#undef lua_replace


enum class Hooks {
  Call      = LUA_HOOKCALL,
  Return    = LUA_HOOKRET,
  Line      = LUA_HOOKLINE,
  Count     = LUA_HOOKCOUNT,
  TailCall  = LUA_HOOKTAILCALL
};

enum class HookMasks {
  Call    = LUA_MASKCALL,
  Return  = LUA_MASKRET,
  Line    = LUA_MASKLINE,
  Count   = LUA_MASKCOUNT
};

#undef LUA_HOOKCALL
#undef LUA_HOOKRET
#undef LUA_HOOKLINE
#undef LUA_HOOKCOUNT
#undef LUA_HOOKTAILCALL

#undef LUA_MASKCALL
#undef LUA_MASKRET
#undef LUA_MASKLINE
#undef LUA_MASKCOUNT


using Debug = detail::lua_Debug;

using Hook = detail::lua_Hook;


inline bool GetStack(State* L, int level, Debug* ar) {
  return static_cast<bool>(detail::lua_getstack(L, level, ar));
}
inline int GetInfo(State* L, const char* what, Debug* ar) {
  return detail::lua_getinfo(L, what, ar);
}
inline const char* GetLocal(State* L, const Debug* ar, int n) {
  return detail::lua_getlocal(L, ar, n);
}
inline const char* SetLocal(State* L, const Debug* ar, int n) {
  return detail::lua_setlocal(L, ar, n);
}
inline const char* GetUpvalue(State* L, int funcindex, int n) {
  return detail::lua_getupvalue(L, funcindex, n);
}
inline const char* SetUpvalue(State* L, int funcindex, int n) {
  return detail::lua_setupvalue(L, funcindex, n);
}
inline void* UpvalueID(State* L, int fidx, int n) {
  return detail::lua_upvalueid(L, fidx, n);
}
inline void UpvalueJoin(State* L, int fidx1, int n1, int fidx2, int n2) {
  return detail::lua_upvaluejoin(L, fidx1, n1, fidx2, n2);
}
inline void SetHook(State* L, Hook func, int mask, int count) {
  return detail::lua_sethook(L, func, mask, count);
}
inline Hook GetHook(State* L) {
  return detail::lua_gethook(L);
}
inline HookMasks GetHookMask(State* L) {
  return static_cast<HookMasks>(detail::lua_gethookmask(L));
}
inline int GetHookCount(State* L) {
  return detail::lua_gethookcount(L);
}


//TODO: What should be done about lualib.h?
namespace Lib {

namespace Open {

inline int Base(State* L) {
  return detail::luaopen_base(L);
}
inline int Coroutine(State* L) {
  return detail::luaopen_coroutine(L);
}
inline int Table(State* L) {
  return detail::luaopen_table(L);
}
inline int IO(State* L) {
  return detail::luaopen_io(L);
}
inline int OS(State* L) {
  return detail::luaopen_os(L);
}
inline int String(State* L) {
  return detail::luaopen_string(L);
}
inline int Utf8(State* L) {
  return detail::luaopen_utf8(L);
}
inline int Bit32(State* L) {
  return detail::luaopen_bit32(L);
}
inline int Math(State* L) {
  return detail::luaopen_math(L);
}
inline int Debug(State* L) {
  return detail::luaopen_debug(L);
}
inline int Package(State* L) {
  return detail::luaopen_package(L);
}
//Not sure it's the right place for this function since it's an auxiliary
//function, but declared in lualib.h
inline void Libs(State* L) {
  detail::luaL_openlibs(L);
}

} //Namespace Open

} //Namespace Lib

namespace Aux {

using Reg = detail::luaL_Reg;

constexpr auto NumSizes = (sizeof(Integer) * 16) + sizeof(Number);

inline void CheckVersion(State* L, Number ver, size_t sz) {
  detail::luaL_checkversion_(L, ver, sz);
}
inline void CheckVersion(State* L) {
  CheckVersion(L, VersionNumber, NumSizes);
}
#undef luaL_checkversion



inline Types GetMetaField(State* L, int obj, const char* e) {
  return static_cast<Types>(detail::luaL_getmetafield(L, obj, e));
}
inline bool CallMeta(State* L, int obj, const char* e) {
  return static_cast<bool>(detail::luaL_callmeta(L, obj, e));
}
inline const char* ToLString(State* L, int idx, size_t& len) {
  return detail::luaL_tolstring(L, idx, &len);
}
inline int ArgError(State* L, int arg, const char* extramsg) {
  return detail::luaL_argerror(L, arg, extramsg);
}
inline const char* CheckLString(State* L, int arg, size_t& l) {
  return detail::luaL_checklstring(L, arg, &l);
}
inline const char* OptLString(State* L, int arg, const char* def, size_t& l) {
  return detail::luaL_optlstring(L, arg, def, &l);
}
inline Number CheckNumber(State* L, int arg) {
  return detail::luaL_checknumber(L, arg);
}
inline Number OptNumber(State* L, int arg, Number def) {
  return detail::luaL_optnumber(L, arg, def);
}
inline Integer CheckInteger(State* L, int arg) {
  return detail::luaL_checkinteger(L, arg);
}
inline Integer OptInteger(State* L, int arg, Integer def) {
  return detail::luaL_optinteger(L, arg, def);
}
inline void CheckStack(State* L, int sz, const char* msg) {
  detail::luaL_checkstack(L, sz, msg);
}
inline void CheckType(State* L, int arg, int t) {
  detail::luaL_checktype(L, arg, t);
}
inline void CheckAny(State* L, int arg) {
  detail::luaL_checkany(L, arg);
}
inline int NewMetaTable(State* L, const char* tname) {
  return detail::luaL_newmetatable(L, tname);
}
inline void SetMetaTable(State* L, const char* tname) {
  detail::luaL_setmetatable(L, tname);
}
inline void* TestUData(State* L, int ud, const char* tname) {
  return detail::luaL_testudata(L, ud, tname);
}
inline void* CheckUData(State* L, int ud, const char* tname) {
  return detail::luaL_checkudata(L, ud, tname);
}
inline void Where(State* L, int lvl) {
  detail::luaL_where(L, lvl);
}
template<class ...VarArgs>
inline int Error(State* L, const char* fmt, VarArgs&&... args) {
  return detail::luaL_error(L, fmt, args...);
}
template<size_t N>
inline int CheckOption(State* L, int arg, const char* def, const char* const (&lst)[N]) {
  return detail::luaL_checkoption(L, arg,  def, lst);
}
inline int FileTesult(State* L, int stat, const char* fname) {
  return detail::luaL_fileresult(L, stat, fname);
}
inline int ExecResult(State* L, int stat) {
  return detail::luaL_execresult(L, stat);
}



constexpr auto Noref  = -2;
constexpr auto Refnil = -1;

#undef LUA_NOREF
#undef LUA_REFNIL

inline int Ref(State* L, int t) {
  return detail::luaL_ref(L, t);
}
inline void UnRef(State* L, int t, int ref) {
  return detail::luaL_unref(L, t, ref);
}
template<size_t N>
inline Statuses LoadFileX(State* L, const char* filename, const char (&mode)[N]) {
  static_assert(::detail::LoadModeChecker(mode), R"(Load mode can only be "b", "t", "bt" or null pointer)");
  return static_cast<Statuses>(detail::luaL_loadfilex(L, filename, mode));
}
inline Statuses LoadFileX(State* L, const char* filename, const char* mode) {
  return static_cast<Statuses>(detail::luaL_loadfilex(L, filename, mode));
}
inline Statuses LoadFile(State* L, const char* filename) {
  return static_cast<Statuses>(LoadFileX(L, filename, nullptr));
}

#undef luaL_loadfile

inline Statuses LoadBufferX(State* L, const char* buff, size_t sz, const char* name, const char* mode) {
  return static_cast<Statuses>(detail::luaL_loadbufferx(L, buff, sz, name, mode));
}
inline Statuses LoadString(State* L, const char* s) {
  return static_cast<Statuses>(detail::luaL_loadstring(L, s));
}
inline State* NewState() {
  return detail::luaL_newstate();
}
inline Integer Len(State* L, int idx) {
  return detail::luaL_len(L, idx);
}
inline const char* GSub(State* L, const char* s, const char* p, const char* r) {
  return detail::luaL_gsub(L, s, p, r);
}
inline void SetFuncs(State* L, const Reg* l, int nup) {
  detail::luaL_setfuncs(L, l, nup);
}
inline bool GetSubTable(State* L, int idx, const char* fname) {
  return static_cast<bool>(detail::luaL_getsubtable(L, idx, fname));
}
inline void TraceBack(State* L, State* L1, const char* msg, int level) {
  detail::luaL_traceback(L, L1, msg, level);
}
inline void RequiRef(State* L, const char* modname, CFunction openf, bool glb) {
  detail::luaL_requiref(L, modname, openf, static_cast<int>(glb));
}

template<size_t N>
inline void NewLibTable(State* L, const Reg (&l)[N]) {
  CreateTable(L, 0, sizeof(l)/sizeof(l[0]) - 1);
}
template<size_t N>
inline void NewLib(State* L, const Reg (&l)[N]) {
  CheckVersion(L);
  NewLibTable(L, l);
  SetFuncs(L, l, 0);
}

inline void ArgCheck(State* L, bool cond, int arg, const char *extramsg) {
    if(not cond) {
      ArgError(L, arg, extramsg);
    }
}
inline const char* CheckString(State* L, int arg) {
  return detail::luaL_checklstring(L, arg, nullptr);
}
inline const char* OptString(State* L, int arg, const char* d) {
  return detail::luaL_optlstring(L, arg, d, nullptr);
}

inline const char* TypeName(State* L, int index) {
  return ::Lua::TypeName(L, Type(L, index));
}

inline bool DoFile(State* L, const char* filename) {
  return LoadFile(L, filename) == Statuses::OK ? (PCall(L, 0, MultiReturn, 0) == Statuses::OK ? false : true) : true;
}

inline bool DoString(State* L, const char* str) {
  return LoadString(L, str) == Statuses::OK ? (PCall(L, 0, MultiReturn, 0) == Statuses::OK ? false : true) : true;
}

inline Types GetMetaTable(State* L, const char* tname) {
  return GetField(L, RegistryIndex, tname);
}

//TODO: Don't miss this
#define luaL_opt(L,f,n,d) (lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))

inline Statuses LoadBuffer(State* L, const char* buff, size_t sz, const char* name) {
  return LoadBufferX(L, buff, sz, name, nullptr);
}

#undef luaL_newlibtable
#undef luaL_newlib
#undef luaL_argcheck
#undef luaL_checkstring
#undef luaL_optstring
#undef luaL_typename
#undef luaL_dofile
#undef luaL_dostring
#undef luaL_getmetatable
#undef luaL_opt
#undef luaL_loadbuffer
#undef luaL_newlibtable
#undef luaL_newlib
#undef luaL_argcheck
#undef luaL_checkstring
#undef luaL_optstring
#undef luaL_typename
#undef luaL_dofile
#undef luaL_dostring
#undef luaL_getmetatable
#undef luaL_opt
#undef luaL_loadbuffer


using Buffer = detail::luaL_Buffer;

inline void AddSize(Buffer& B, size_t n) {
  luaL_addsize(&B, n);
}
#undef luaL_addsize

inline void BuffInit(State* L, Buffer& B) {
  detail::luaL_buffinit(L, &B);
}
inline char* PrepBuffSize(Buffer& B, size_t sz) {
  return detail::luaL_prepbuffsize(&B, sz);
}
inline void AddLString(Buffer& B, const char* s, size_t l) {
  detail::luaL_addlstring(&B, s, l);
}
inline void AddString(Buffer& B, const char* s) {
  detail::luaL_addstring(&B, s);
}
inline void AddValue(Buffer& B) {
  detail::luaL_addvalue(&B);
}
inline void PushResult(Buffer& B) {
  detail::luaL_pushresult(&B);
}
inline void PushResultSize(Buffer& B, size_t sz) {
  detail::luaL_pushresultsize(&B, sz);
}
inline char* BuffInitSize(State* L, Buffer& B, size_t sz) {
  return detail::luaL_buffinitsize(L, &B, sz);
}

#define luaL_prepbuffsize(B, sz) PrepBuffSize(*B, sz)
inline void AddChar(Buffer& B, char c) {
  luaL_addchar(&B, c);
  //B.n < B.size || PrepBuffSize(B, 1), B.b[B.n++] = c;
}
#undef luaL_prepbuffsize
#undef luaL_addchar


inline char* PrepBuffer(Buffer& B) {
  return PrepBuffSize(B, LUAL_BUFFERSIZE);
}
#undef luaL_prepbuffer

} //Namepsace Aux

} //Namespace Lua

#endif /* LUA_HPP */
