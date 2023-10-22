#include "Application.h"
#include "Window/Window.h"
#include "Scene/Scene.h"
#include "Components.h"
#include <iostream>
#include <format>
#include <assimp/Importer.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "GLFW/glfw3.h"
#include "Graphics/DebugRenderer.h"
#include "System/TransformSystem.h"
#include "System/Graphics/AnimationSystem.h"
#include "System/Graphics/SceneCamera.h"
#include "System/Graphics/SceneRenderer.h"
#include "System/Graphics/SceneViewer.h"
#include "Window/GLFW.h"


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
	/*m_World.add<ActiveScene>();

	m_World.observer<ActiveScene>("Scene Change to Game")
		.event(flecs::OnAdd)
		.second<Scene>()
		.each([](flecs::iter& it, size_t, ActiveScene)
		{
			flecs::world world = it.world();
			flecs::entity scene = world.component<SceneRoot>();
			//std::cout << ">> ActiveScene has changed to";
			world.set_pipeline(world.get<Scene>()->pipeline);
		});


	m_World.component<SceneRoot>();

	m_World.delete_with(flecs::ChildOf, m_World.entity<SceneRoot>());

	flecs::entity game = m_World.pipeline()
		.with(flecs::System)
		.with()
		.build();

	// Set pipeline entities on the scenes
   // to easily find them later with get().
	m_World.set<Scene>({ game });

	m_World.add<ActiveScene, Scene>();

	auto object=m_World.entity("object");
	object.add<Translation>();

	m_World.system<Translation>("print").kind(flecs::OnStart).iter([](flecs::iter iter)
	{
		for(auto i:iter)
			std::cout << iter.entity(i).name() << std::endl;
	});

	Assimp::Importer importer;*/

	m_Window->Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_Window->GetRawHandle()), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	glClearColor(0.2f, 0.2f, 1.f, 1.f);
	DebugRenderer::Init();

	//register systems
	m_World.add<MainCamera>();
	m_World.entity("MainCamera").set<Transform>({ {0,0,50.f} }).add<Camera>();
	AddSystem<TransformSystem>();
	AddSystem<AnimationSystem>();
	AddSystem<SceneCamera>();
	AddSystem<SceneRenderer>();
	AddSystem<SceneViewer>();

	m_Window->Run([&]()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		m_World.progress();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	});

	m_Window->Shutdown();
	return 0;
}

flecs::world& Application::GetWorld()
{
	return m_World;
}

std::shared_ptr<Window> Application::GetWindow()
{
	return m_Window;
}
