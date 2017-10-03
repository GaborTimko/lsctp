project('lsctp', 'cpp', default_options : ['cpp_std=c++14'])

lua = subproject('lua')

#libsctp = dependency('sctp', version : '>=1.0.17')
shared_library(
  'sctp',
  'src/lsctp.cpp',
  name_prefix : '',
  cpp_args : ['-Og', '-Wall', '-Wextra', '-Wformat', '-pedantic', '-fno-exceptions', '-fno-rtti', '--coverage'],
  include_directories : include_directories('include'),
  cpp_pch : 'pch/PreComp.hpp',
  link_args: ['-lsctp', '--coverage'],
  link_with : lua.get_variable('lualib'),
)

test(
  'Sctp test',
  lua.get_variable('luaInterpreter'),
  args : ['../tst/lua/socket.lua']
)
