-- premake.lua - public domain
-- authored from 2012-2013 by Adrien Herubel 

solution "imgui"
   configurations { "Debug", "Release" }
   platforms {"native", "x64", "x32"}

  
   -- imgui sample_gl3
   project "sample_gl3"
      kind "ConsoleApp"
      language "C++"
      files { "sample_gl3.cpp", "imgui.cpp",  "imguiRenderGL3.cpp",  "imgui.h",  "imguiRenderGL3.h",  "stb_truetype.h" }
      includedirs { "lib/glfw/include", "src", "common", "lib/" }
      links {"glfw", "glew"}
      defines { "GLEW_STATIC" }
     
      configuration { "linux" }
         links {"X11","Xrandr", "rt", "GL", "GLU", "pthread"}
       
      configuration { "windows" }
         links {"glu32","opengl32", "gdi32", "winmm", "user32"}

      configuration { "macosx" }
         linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit" }
       
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "ExtraWarnings"}

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "ExtraWarnings"}    


   -- GLFW Library
   project "glfw"
      kind "StaticLib"
      language "C"
      files { "lib/glfw/lib/*.h", "lib/glfw/lib/*.c", "lib/glfw/include/GL/glfw.h" }
      includedirs { "lib/glfw/lib", "lib/glfw/include"}

      configuration {"linux"}
         files { "lib/glfw/lib/x11/*.c", "lib/glfw/x11/*.h" }
         includedirs { "lib/glfw/lib/x11" }
         defines { "_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD", "_GLFW_HAS_GLXGETPROCADDRESS" }
         buildoptions { "-pthread" }
       
      configuration {"windows"}
         files { "lib/glfw/lib/win32/*.c", "lib/glfw/win32/*.h" }
         includedirs { "lib/glfw/lib/win32" }
         defines { "_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD", "_GLFW_HAS_GLXGETPROCADDRESS" }
       
      configuration {"Macosx"}
         files { "lib/glfw/lib/cocoa/*.c", "lib/glfw/lib/cocoa/*.h", "lib/glfw/lib/cocoa/*.m" }
         includedirs { "lib/glfw/lib/cocoa" }
         defines { }
         buildoptions { " -fno-common" }
         linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "ExtraWarnings" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "ExtraWarnings" }    

   -- GLEW Library         
   project "glew"
      kind "StaticLib"
      language "C"
      files {"lib/glew/*.c", "lib/glew/*.h"}
      defines { "GLEW_STATIC" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "ExtraWarnings" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "ExtraWarnings" }    
