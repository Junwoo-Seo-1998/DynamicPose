workspace "JEngine"
    platforms {"Win64"}
    configurations  { "Debug", "Release" }    
    startproject "JEngine"

group "Libs"
include "Libs/glfw"
include "Libs/glad"
include "Libs/imgui"
include "Libs/flecs"
group ""

--inc
IncludeDir={}
IncludeDir["glfw"]="Libs/glfw/glfw/include/"
IncludeDir["glad"]="Libs/glad/"
IncludeDir["imgui"]="Libs/imgui/"
IncludeDir["flecs"]="Libs/flecs/flecs/include/"
IncludeDir["assimp"]="Libs/assimp/include/"
--lib dir
LibraryDir = {}
LibraryDir["D_assimp"]="Libs/assimp/dbg"
LibraryDir["assimp"]="Libs/assimp/rel"
--lib
Library = {}
Library["D_assimp"]="assimp-vc143-mtd.lib"
Library["assimp"]="assimp-vc143-mt.lib"
--dll files
Shared = {}
Shared["D_assimp"]="%{wks.location}/Libs/assimp/dbg/assimp-vc143-mtd.dll"
Shared["assimp"]="%{wks.location}/Libs/assimp/rel/assimp-vc143-mt.dll"

group "Engine"
project "JEngineCore"
    location "JEngineCore"
    kind "StaticLib"
    language "C++"
    cppdialect "c++20"
    architecture "x86_64"

    flags
    {
        "MultiProcessorCompile",
    }

    links {
        "glfw",
        "glad",
        "opengl32.lib",
        "imgui",
        "flecs"
    }

    disablewarnings {
        "4819","4996","4005"
    }
    linkoptions {}
    includedirs
    {
        "JEngineCore",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.flecs}",
        "%{IncludeDir.assimp}",
    }
    files {
        "JEngineCore/src/**.h",
        "JEngineCore/src/**.c",
        "JEngineCore/src/**.cpp",
        "JEngineCore/src/**.hpp",
    }
    defines{
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "flecs_STATIC",
    }

    postbuildcommands {

    }

    filter "system:windows"
        linkoptions { "-IGNORE:4006",}
    -- All of these settings will appear in the Debug configuration
    filter "configurations:Debug"
        defines { "DEBUG", "JJ_DEBUG" }
        symbols "On"

        libdirs 
        {
            "%{LibraryDir.D_assimp}"
        }

        links {
            "%{Library.D_assimp}"
        }
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

        libdirs 
        {
            "%{LibraryDir.assimp}"
        }

        links {
            "%{Library.assimp}"
        }
        

project "JEngine"
    location "JEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "c++20"
    architecture "x86_64"
    
    flags
    {
        "MultiProcessorCompile"
    }
    
    links 
    {
        "JEngineCore",
        "glad",
        "opengl32.lib",
        "imgui",
        "flecs"
    }
        
    disablewarnings
    {
        "4819","4996","4005"
    }
    linkoptions {}
    includedirs
    {
        "JEngine",
        "JEngine/src",
        "JEngineCore/src",
        "%{IncludeDir.glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.flecs}",
    }
    files 
    {
        "JEngine/src/**.h",
        "JEngine/src/**.c",
        "JEngine/src/**.cpp",
        "JEngine/src/**.hpp",
    }
    defines
    {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
        "flecs_STATIC",
    }
        
    postbuildcommands 
    {
        --"{COPYDIR} \"../RuntimeDependencies/\" \"../bin/%{cfg.buildcfg}/\"",
    }
    
    debugdir "RuntimeDependencies"
    
    -- All of these settings will appear in the Debug configuration
    filter "configurations:Debug"
        defines { "DEBUG", "J_DEBUG" }
        symbols "On"

        postbuildcommands 
        {
            '{COPYFILE} %{Shared.D_assimp} %{cfg.buildtarget.directory}/assimp-vc143-mtd.dll',
        }
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

        postbuildcommands 
        {
            '{COPYFILE} %{Shared.assimp} %{cfg.buildtarget.directory}/assimp-vc143-mt.dll',
        }
group ""