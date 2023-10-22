#pragma once
#include <memory>
#include <flecs.h>
#include <vector>

#include "System/System.h"

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
	template<typename SystemClass>
	void AddSystem();

	int Run();

	flecs::world& GetWorld();
	std::shared_ptr<Window> GetWindow();
private:
	std::shared_ptr<Window> m_Window;
	flecs::world m_World{};

	std::vector<std::shared_ptr<System>>m_Systems;
};

template <typename SystemClass>
void Application::AddSystem()
{
	std::shared_ptr<System> system = std::make_shared<SystemClass>();
	m_Systems.push_back(system);
	system->RegisterSystem(m_World);
}

