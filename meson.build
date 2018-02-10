project('lsctp', 'cpp', default_options : ['cpp_std=c++14'])

lua = subproject('lua')

#libsctp = dependency('sctp', version : '>=1.0.17')
shared_library(
  'sctp',
  'src/lsctp.cpp',
  name_prefix : '',
  cpp_args : '-Wall -Wextra -Wformat -pedantic -march=native --coverage'.split(),
  include_directories : include_directories('include'),
  link_args: ['-lsctp', '--coverage'],
  link_with : lua.get_variable('lualib'),
  install : true,
  install_dir : get_option('prefix') + '/bin'
)

test(
  'Sctp test',
  lua.get_variable('luaInterpreter'),
  args : ['../tst/lua/socket.lua']
)
