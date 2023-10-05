project "flecs"
    kind "StaticLib"
    language "C++"
    cppdialect "c++17"
    staticruntime "off"
    
    --targetdir ("bin/".."%{prj.name}")
    --objdir ("bin-int/".."%{prj.name}")

    disablewarnings {
        "4819",
    }

    includedirs
    {
        "flecs/include",
    }

    files
    {
        "flecs/include/**.h",
        "flecs/include/**.c",
        "flecs/include/**.cpp",
        "flecs/include/**.hpp",

        "flecs/src/**.h",
        "flecs/src/**.c",
        "flecs/src/**.cpp",
        "flecs/src/**.hpp",
    }
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "flecs_STATIC"
    }
    filter "system:windows"
        flags
        {
            "MultiProcessorCompile"
        }

        systemversion "latest"
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines { "_DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines { "NDEBUG" }