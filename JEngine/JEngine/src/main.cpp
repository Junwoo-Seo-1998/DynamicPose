#include "Application.h"
#include "Window/GLFW.h"
#include <filesystem>
int main([[maybe_unused]] int argc, [[maybe_unused]] char** args)
{
	std::filesystem::current_path(std::filesystem::path(args[0]).parent_path());
	ApplicationBuilder builder;
	builder.SetWindow<GLFW>();
	auto app = Application::CreateInstance(builder);
	
	return app->Run();
}
