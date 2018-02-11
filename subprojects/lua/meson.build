project('lua', 'c', default_options : ['c_std=c99'])

luaIncludes = include_directories('src')

luaSources = [
  'src/lapi.c',
  'src/lauxlib.c',
  'src/lbaselib.c',
  'src/lbitlib.c',
  'src/lcode.c',
  'src/lcorolib.c',
  'src/lctype.c',
  'src/ldblib.c',
  'src/ldebug.c',
  'src/ldo.c',
  'src/ldump.c',
  'src/lfunc.c',
  'src/lgc.c',
  'src/linit.c',
  'src/liolib.c',
  'src/llex.c',
  'src/lmathlib.c',
  'src/lmem.c',
  'src/loadlib.c',
  'src/lobject.c',
  'src/lopcodes.c',
  'src/loslib.c',
  'src/lparser.c',
  'src/lstate.c',
  'src/lstring.c',
  'src/lstrlib.c',
  'src/ltable.c',
  'src/ltablib.c',
  'src/ltm.c',
  'src/lundump.c',
  'src/lutf8lib.c',
  'src/lvm.c',
  'src/lzio.c',
]

lualib = shared_library('lua53',
  luaSources,
  include_directories : luaIncludes,
  c_args : '-DLUA_USE_LINUX -Wextra -Wall'.split(),
  link_args: '-lm -ldl -lreadline -Wl,-E'.split(),
  install : true,
  install_dir : get_option('prefix') + '/bin'
)

luadep = declare_dependency(
  include_directories : luaIncludes,
  link_with : lualib,
  version : '5.3'
)

luaInterpreter = executable(
  'lua',
  'src/lua.c',
  dependencies : luadep,
  link_args: ['-lm -ldl -lreadline -Wl,-E'.split(),
  #install : true,
  #install_dir : get_option('prefix') + '/bin'
)
