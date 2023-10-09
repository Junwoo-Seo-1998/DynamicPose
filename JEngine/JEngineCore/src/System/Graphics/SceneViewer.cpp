#include "SceneViewer.h"

#include <iostream>

#include "imgui.h"
#include "Components.h"
void SceneViewer::RegisterSystem(flecs::world& _world)
{

	_world.system<Transform>("SceneViewer").kind(flecs::OnStore).iter([&](flecs::iter iter)
	{
		RenderSceneGUI(iter);
	});
}

void iterate_tree(flecs::entity e) {
	std::string str = e.name().c_str();
	if(str=="")
		return;
	if (ImGui::TreeNodeEx(str.c_str()))
	{
		// Iterate children recursively
		e.children([&](flecs::entity child) {
			iterate_tree(child);
			});
		ImGui::TreePop();
	}
}

void SceneViewer::RenderSceneGUI(flecs::iter iter)
{
	ImGui::Begin("SceneViewer");
	for (int i:iter)
	{
		flecs::entity e = iter.entity(i);
		if (!e.parent().is_valid())
		{
			iterate_tree(e);
		}
	}

	ImGui::End();
}
