project('lsctp', ['c', 'cpp'], default_options : ['c_std=c99', 'cpp_std=c++14'])

luaSources = [
  'lua/src/lapi.c',
  'lua/src/lauxlib.c',
  'lua/src/lbaselib.c',
  'lua/src/lbitlib.c',
  'lua/src/lcode.c',
  'lua/src/lcorolib.c',
  'lua/src/lctype.c',
  'lua/src/ldblib.c',
  'lua/src/ldebug.c',
  'lua/src/ldo.c',
  'lua/src/ldump.c',
  'lua/src/lfunc.c',
  'lua/src/lgc.c',
  'lua/src/linit.c',
  'lua/src/liolib.c',
  'lua/src/llex.c',
  'lua/src/lmathlib.c',
  'lua/src/lmem.c',
  'lua/src/loadlib.c',
  'lua/src/lobject.c',
  'lua/src/lopcodes.c',
  'lua/src/loslib.c',
  'lua/src/lparser.c',
  'lua/src/lstate.c',
  'lua/src/lstring.c',
  'lua/src/lstrlib.c',
  'lua/src/ltable.c',
  'lua/src/ltablib.c',
  'lua/src/ltm.c',
  'lua/src/lundump.c',
  'lua/src/lutf8lib.c',
  'lua/src/lvm.c',
  'lua/src/lzio.c',
]

#libm = dependency('m')
#libdl = dependency('dl')
#libreadline = dependency('readline')

lualib = shared_library('lua53',
  luaSources,
  include_directories : include_directories('lua/src'),
  c_args : ['-DLUA_USE_LINUX', '-Wextra', '-Wall'],
  #dependencies : [libm, libdl, libreadline],
  link_args: ['-lm', '-ldl', '-lreadline', '-Wl,-E']
)

luaInterpreter =  executable('lua', 'lua/src/lua.c', link_with : lualib)

lSctpSources = [
  'src/lsctp.cpp',
  'src/SctpSocket.cpp',
]

#libsctp = dependency('sctp', version : '>=1.0.17')
shared_library('sctp',
  lSctpSources,
  name_prefix : '',
  cpp_args : ['-Wall', '-Wextra', '-Wformat', '-pedantic', '-fno-exceptions', '-fno-rtti', '--coverage'],
  link_with : lualib,
  include_directories : include_directories('include'),
  #dependencies : libsctp
  link_args: ['-lsctp', '--coverage']
)

test(
  'Sctp test',
  luaInterpreter,
  args : ['../tst/lua/socket.lua']
)
