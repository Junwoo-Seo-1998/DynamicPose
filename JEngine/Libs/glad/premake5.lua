project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    systemversion "latest"

    includedirs
    {
        "glad",
    }

    files{
        "glad/**.h",
        "glad/**.c"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        runtime "Release"
        optimize "on"