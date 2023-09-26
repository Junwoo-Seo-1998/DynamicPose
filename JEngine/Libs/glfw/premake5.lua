project "glfw"
    kind "StaticLib"
    language "C"

    --common
    files
    {
        "glfw/include/GLFW/glfw3.h",
        "glfw/include/GLFW/glfw3native.h",
        "glfw/src/context.c",
        "glfw/src/init.c",
        "glfw/src/input.c",
        "glfw/src/monitor.c",
        "glfw/src/vulkan.c",
        "glfw/src/window.c",
    }

    filter "system:windows"
        --windows only
        files --headers
        {
            "glfw/src/win32_platform.h",
            "glfw/src/win32_joystick.h",
            "glfw/src/wgl_context.h",
            "glfw/src/egl_context.h",
            "glfw/src/osmesa_context.h",
        }

        files --srcs
        {
            "glfw/src/win32_init.c",
            "glfw/src/win32_joystick.c",
            "glfw/src/win32_monitor.c",
            "glfw/src/win32_time.c",
            "glfw/src/win32_thread.c",
            "glfw/src/win32_window.c",
            "glfw/src/wgl_context.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c",
        }

        flags
        {
            "MultiProcessorCompile"
        }

        systemversion "latest"
        staticruntime "On"
        disablewarnings { "4819", "4002", "4005" }
        

        defines
        {
            "_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
        }

        filter "configurations:Debug"
		    runtime "Debug"
		    symbols "on"

	    filter "configurations:Release"
		    runtime "Release"
		    optimize "on"