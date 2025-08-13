workspace("GLEngine")

    architecture "x64"
	configurations {"Debug", "Release"}
language "C++"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "HelloGL"
	kind "ConsoleApp"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-obj/" .. outputdir .. "/%{prj.name}")

   files { "**.h", "**.cpp" }
   removefiles {
    "ext/opengl/glm/**.cpp"
    }

   includedirs {
       "ext/opengl/glew/include",
       "ext/opengl/glfw/include",
       "ext/opengl/",
       "ext/stb"
   }

   libdirs {
        "ext/opengl/glew/lib",
        "ext/opengl/glfw/lib"
   }

   links {
       "opengl32",
       "glew32s",
       "glfw3"
   }

   defines { "GLEW_STATIC", 
   "STB_PERLIN_IMPLEMENTATION"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"