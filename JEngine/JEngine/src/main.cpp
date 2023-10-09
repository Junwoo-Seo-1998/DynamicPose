#include "Application.h"
#include "Window/GLFW.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** args)
{
	ApplicationBuilder builder;
	builder.SetWindow<GLFW>();
	auto app = Application::CreateInstance(builder);
	
	return app->Run();
}
