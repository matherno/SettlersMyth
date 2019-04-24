workspace "SettlersMyth"
   configurations { "debug", "release" }

project "SettlersMyth"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin_%{cfg.buildcfg}"
   
   files { "src/**.h", "src/**.cpp" }
   
   libdirs { "./dependencies/mathernogl" }

   links { "GL", "GLEW", "glfw", "freeimage" }
   
   includedirs { "./dependencies", "./dependencies/mathernogl", "./src", "./dependencies/misc" }

   cppdialect "C++11"

   filter "configurations:debug"
      defines { "DEBUG" }
      symbols "On"
      links { "MathernoGL_DEBUG" }

   filter "configurations:release"
      defines { "NDEBUG" }
      optimize "On"
      links { "MathernoGL" }

   filter {}
      links { "GL", "GLEW", "glfw", "freeimage" }
