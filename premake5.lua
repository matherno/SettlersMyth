workspace "SettlersMyth"
   configurations { "debug", "release" }

project "SettlersMyth"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin_%{cfg.buildcfg}"
   
   files { "src/**.h", "src/**.cpp" }
   
   libdirs { "./dependencies/mathernogl", "./dependencies/TowOff", "./dependencies/RaySplay/"}

   links { "TowOffLib", "MathernoGL", "RaySplayLib", "GL", "GLEW", "glfw", "freeimage" }
   
   includedirs { "./dependencies", "./dependencies/TowOff", "./src", "./dependencies/RaySplay/include", "./dependencies/misc" }

   cppdialect "C++11"

   filter "configurations:debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:release"
      defines { "NDEBUG" }
      optimize "On"