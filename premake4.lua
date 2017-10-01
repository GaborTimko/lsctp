solution "lsctp"
  configurations { "debug", "release" }
  
  project "lsctp"
    language "C++"
    kind "SharedLib"
    location "build"
    includedirs "include"
    pchheader "include/PreComp.hpp"
    files "src/*.cpp"
    targetprefix ""
    targetname "sctp"
    -- libdirs "lib"
    links { "lua", "sctp" }

    configuration "debug"
      flags {  "NoExceptions", "NoRTTI" }
      buildoptions { "-Og -g" }
      targetdir "bin/debug"

    configuration "release"
      defines "NDEBUG"
      flags { "NoExceptions", "NoRTTI" }
      buildoptions { "-O3 -s" }
      targetdir "bin/release"

    configuration "gmake"
      buildoptions { "-Wall -Wextra -Wformat -pedantic -std=c++14" }
