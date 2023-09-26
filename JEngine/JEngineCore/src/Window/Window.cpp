#include "Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
void Window::Init()
{
	if (!glfwInit())
	{
		// Initialization failed
	}
}

void Window::Shutdown()
{
	glfwTerminate();
}
