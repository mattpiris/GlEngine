workspace("GLEngine")

    architecture "x64"
	configurations {"Debug", "Release"}
language "C++"
    cppdialect "C++20"

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
       "ext/assimp/",
       "ext/opengl/",
       "ext/imgui",
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

   defines { "GLEW_STATIC" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      libdirs {"ext/assimp/lib/Debug", "ext/zlib/lib"}
      links { "assimp-vc143-mtd", "zlibd" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      libdirs {"ext/assimp/lib/Release", "ext/zlib/lib"}
      links { "assimp-vc143-mt", "zlib"}