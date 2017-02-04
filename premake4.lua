solution "lsctp"
  configurations { "debug", "release" }
  
  project "lsctp"
    language "C++"
    kind "SharedLib"
    location "build"
    includedirs "include"
    files "src/*.cpp"
    targetprefix ""
    targetname "sctp"
    -- libdirs "lib"
    links "lua"

    configuration "debug"
      flags { "Symbols", "NoImportLib" }
      targetdir "bin/debug"

    configuration "release"
      defines "NDEBUG"
      flags { "Optimize", "NoImportLib" }
      targetdir "bin/release"

    configuration "gmake"
      buildoptions { "-Wall -Wextra -Wformat -pedantic -std=c++14 -fPIC" }
