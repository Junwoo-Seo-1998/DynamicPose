#include "GLFW.h"
#include "Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input.h"
void KeyCallback(GLFWwindow* _pWindow, int _key, int _scancode, int _action, int _mods)
{

}
bool GLFW::Init()
{
    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    {
        GLFWwindow* window = glfwCreateWindow(640, 640, "CS460", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return false;
        }

        m_Window = std::shared_ptr<GLFWwindow>(window, [](GLFWwindow* windowPtr)
        {
        	glfwDestroyWindow(windowPtr);
        });
    }
    glfwSetKeyCallback(m_Window.get(),Input::GLFWKeyCallback);
    glfwSetMouseButtonCallback(m_Window.get(), Input::GLFWMouseCallback);
    glfwSetScrollCallback(m_Window.get(), Input::GLFWMouseScrollCallback);
    glfwSetCursorPosCallback(m_Window.get(), Input::GLFWMousePositionCallback);
    glfwMakeContextCurrent(m_Window.get());
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void GLFW::Shutdown()
{
    //custom deleter will handle
    m_Window = nullptr;
    glfwTerminate();
}

void GLFW::Run(std::function<void()> callback)
{
    int width, height;
    while (!glfwWindowShouldClose(m_Window.get()))
    {
        //todo: remove later
        glfwGetFramebufferSize(m_Window.get(), &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        callback();

        glfwSwapBuffers(m_Window.get());
        Input::Reset();
        glfwPollEvents();
    }
}

void* GLFW::GetRawHandle()
{
    return m_Window.get();
}

std::tuple<int, int> GLFW::GetWindowSize()
{
    int width, height;
    glfwGetFramebufferSize(m_Window.get(), &width, &height);
    return { width, height };
}
