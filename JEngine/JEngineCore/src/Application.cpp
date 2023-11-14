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
#include "System/CurveSystem.h"
#include "System/DemoGUI.h"
#include "System/TransformSystem.h"
#include "System/Graphics/AnimationSystem.h"
#include "System/Graphics/SceneCamera.h"
#include "System/Graphics/SceneRenderer.h"
#include "System/Graphics/SceneViewer.h"
#include "Window/GLFW.h"


static std::shared_ptr<Application> s_Application;

std::shared_ptr<Application> Application::CreateInstance(const ApplicationBuilder& builder)
{
	s_Application = std::make_shared<Application>();
	s_Application->m_Window = builder.m_Window;
	return s_Application;
}

Application& Application::Get()
{
	return *s_Application;
}

int Application::Run()
{
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

	glClearColor(0.2f, 0.2f, 0.8f, 1.f);
	DebugRenderer::Init();

	//register systems
	m_World.add<Config>();
	m_World.add<MainCamera>();
	m_World.entity("MainCamera").set<Transform>({ {0.f,2.f,2.f}, 
		glm::quat(glm::radians(glm::vec3(-30.f, 0.f, 0.f))),
	}).add<Camera>();

	AddSystem<TransformSystem>();
	AddSystem<AnimationSystem>();
	AddSystem<CurveSystem>();
	AddSystem<SceneCamera>();
	AddSystem<SceneRenderer>();
	AddSystem<SceneViewer>();
	AddSystem<DemoGUI>();

	m_Window->Run([&]()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Run All Registered Systems
		m_World.progress();

		// GUI Rendering
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
