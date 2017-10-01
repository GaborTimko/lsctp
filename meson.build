project('lsctp', 'cpp', default_options : ['cpp_std=c++14'])

subproject('lua')

liblua = dependency('lua')

#libsctp = dependency('sctp', version : '>=1.0.17')
shared_library(
  'sctp',
  'src/lsctp.cpp',
  name_prefix : '',
  cpp_args : ['-Og', '-Wall', '-Wextra', '-Wformat', '-pedantic', '-fno-exceptions', '-fno-rtti', '--coverage'],
  #link_with : lualib,
  include_directories : include_directories('include'),
  cpp_pch : 'pch/PreComp.hpp',
  dependencies : liblua,
  link_args: ['-lsctp', '--coverage'],
  install_dir : 'bin'
)

test(
  'Sctp test',
  luaInterpreter,
  args : ['../tst/lua/socket.lua']
)
