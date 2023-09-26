workspace "JEngine"
    platforms {"Win64"}
    configurations  { "Debug", "Release" }    
    startproject "JEngine"

group "Libs"
include "Libs/glfw"
include "Libs/glad"
include "Libs/imgui"
group ""

--inc
IncludeDir={}
IncludeDir["glfw"]="Libs/glfw/glfw/include/"
IncludeDir["glad"]="Libs/glad/"
IncludeDir["imgui"]="Libs/imgui/"
--lib dir
LibraryDir = {}
--lib
Library = {}

group "Engine"
project "JEngineCore"
    location "JEngineCore"
    kind "StaticLib"
    language "C++"
    cppdialect "c++20"
    architecture "x86_64"

    flags
    {
        "MultiProcessorCompile"
    }

    links {
        "glfw",
        "glad",
        "opengl32.lib",
        "imgui",
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
    }
    files {
        "JEngineCore/src/**.h",
        "JEngineCore/src/**.c",
        "JEngineCore/src/**.cpp",
        "JEngineCore/src/**.hpp",
    }
    defines{
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
    }

    postbuildcommands {

    }

    filter "system:windows"
        linkoptions { "-IGNORE:4006",}
    -- All of these settings will appear in the Debug configuration
    filter "configurations:Debug"
        defines { "DEBUG", "JJ_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

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
    }
        
    disablewarnings
    {
        "4819","4996","4005"
    }
    linkoptions {}
    includedirs
    {
        "JEngine",
        "%{IncludeDir.glad}",
        "%{IncludeDir.imgui}",
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
        "_CRT_SECURE_NO_WARNINGS"
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
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
group ""