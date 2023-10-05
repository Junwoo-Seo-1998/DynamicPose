#pragma once
#include <memory>
#include <flecs.h>

class Window;
class Application;
class ApplicationBuilder
{
	friend Application;
public:
	template<typename WindowType>
	ApplicationBuilder& SetWindow();

private:
	std::shared_ptr<Window> m_Window;
};

template <typename WindowType>
ApplicationBuilder& ApplicationBuilder::SetWindow()
{
	m_Window = std::make_shared<WindowType>();
	return *this;
}

class Application
{
public:
	static std::shared_ptr<Application> CreateInstance(const ApplicationBuilder& builder);
	static Application& Get();
	int Run();
private:
	std::shared_ptr<Window> m_Window;
	flecs::world m_World{};
};
