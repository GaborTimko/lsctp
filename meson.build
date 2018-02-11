project('lsctp', 'cpp', default_options : ['cpp_std=c++14'])

libsctp = dependency('libsctp', required : true)
luadep  = dependency('lua', version : '>= 5.3', fallback : ['lua', 'luadep'])

shared_library(
  'sctp',
  'src/lsctp.cpp',
  name_prefix : '',
  dependencies : [libsctp, luadep],
  cpp_args : '-Wall -Wextra -Wformat -pedantic -march=native --coverage'.split(),
  include_directories : include_directories('include'),
  link_args: '--coverage'.split(),
  #install : true,
  #đinstall_dir : get_option('prefix') + '/bin'
)

luaInterpreter = find_program('lua')

if not luaInterpreter.found()
  lua = subproject('lua')
  luaInterpreter = lua.get_variable('luaInterpreter')
endif

test(
  'Sctp test',
  luaInterpreter,
  args : ['../tst/lua/socket.lua']
)
