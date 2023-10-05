#include "Application.h"
#include "Window/Window.h"
#include "Scene/Scene.h"
#include "Components.h"
#include <iostream>
#include <format>
#include <assimp/Importer.hpp>
static std::shared_ptr<Application> s_Application;

std::shared_ptr<Application> Application::CreateInstance(const ApplicationBuilder& builder)
{
	//ASSERT(s_Application == nullptr, "Can Be Create Only Once");
	s_Application = std::make_shared<Application>();
	s_Application->m_Window = builder.m_Window;
	return s_Application;
}

Application& Application::Get()
{
	//ASSERT(s_Application != nullptr, "The Application Has Not Been Created Yet");
	return *s_Application;
}

int Application::Run()
{
	// Can only have one active scene
	// in a game at a time.
	/*m_World.component<ActiveScene>()
		.add(flecs::Exclusive);

	flecs::entity game = m_World.pipeline()
		.with(flecs::System)
		.build();*/

	// Set pipeline entities on the scenes
   // to easily find them later with get().
	//m_World.set<Scene>({ game });

	//m_World.add<ActiveScene, Scene>();

	auto object=m_World.entity("object");
	object.add<Translation>();

	m_World.system<Translation>("print").kind(flecs::OnStart).iter([](flecs::iter iter)
	{
		for(auto i:iter)
			std::cout << iter.entity(i).name() << std::endl;
	});

	Assimp::Importer importer;
	m_Window->Init();

	m_Window->Run([&]()
	{

		m_World.progress();
	});

	m_Window->Shutdown();
	return 0;
}
