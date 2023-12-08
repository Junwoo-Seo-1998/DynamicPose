#include "Demo.h"

#include <format>

#include "Components.h"
#include "flecs.h"
#include "imgui.h"
#include "Input.h"

void Demo::RegisterSystem(flecs::world& _world)
{
	_world.system<Config>("DEMO_GUI").kind(flecs::OnValidate).iter([&](flecs::iter& iter, Config* config)
		{
			DrawGUI(iter, config);
		});

	_world.system<IKGoal>("Set Goal").kind(flecs::OnUpdate).iter([&](flecs::iter& iter, IKGoal* goal)
	{
		UpdateGoal(iter, goal);
	});
}

void Demo::DrawGUI(flecs::iter& iter, Config* config)
{
	ImGui::Begin("DemoGUI");
	for (auto i : iter)
	{
		ImGui::Text(std::format("delta time: {}", iter.delta_time()).c_str());
		ImGui::Checkbox("Show Skeleton", &config[i].ShowSkeleton);
		ImGui::Checkbox("Use VQS", &config[i].UseVQS);
		/*{
			auto& items = config[i].AnimationList;
			static int selectedItem = 16;
			static std::string item_current = items[16]->AnimationName;            // Here our selection is a single pointer stored outside the object.
			if (ImGui::BeginCombo("Animation", item_current.c_str())) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < items.size(); n++)
				{
					bool is_selected = (item_current == items[n]->AnimationName);
					if (ImGui::Selectable(items[n]->AnimationName.c_str(), is_selected))
					{
						item_current = items[n]->AnimationName;
						selectedItem = n;
					}
					if (is_selected)
					{
						
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			auto found = iter.world().lookup("MainModel");
			if(found.is_valid() && found.has<AnimatorComponent>())
				found.get_mut<AnimatorComponent>()->CurrentAnimation = items[selectedItem];
		}*/

		/*auto found = iter.world().lookup("Goal");
		if (found.is_valid())
		{
			float height = found.get_mut<Transform>()->Position.y;
			if (ImGui::DragFloat("Target Height", &height, 0.01f, 1.1f, 1.7f))
			{
				found.get_mut<Transform>()->Position.y=height;
			}
		}*/
		//left
		{
			auto found = iter.world().lookup("fixed_left");
			if (found.is_valid())
			{
				float height = found.get_mut<Transform>()->Position.y;
				if (ImGui::DragFloat("Left Height", &height, 0.1f))
				{
					found.get_mut<Transform>()->Position.y = height;
				}
			}
		}
		//left
		{
			auto found = iter.world().lookup("fixed_right");
			if (found.is_valid())
			{
				float height = found.get_mut<Transform>()->Position.y;
				if (ImGui::DragFloat("Right Height", &height, 0.1f))
				{
					found.get_mut<Transform>()->Position.y = height;
				}
			}
		}
	}
	ImGui::End();
}

void Demo::UpdateGoal(flecs::iter& iter, IKGoal* goal)
{
	for (auto i:iter)
	{
		auto goalEntt=iter.entity(i);
		glm::vec3 movement{ 0,0,0 };
		bool changed = false;
		if (Input::IsPressed(KeyCode::I))
		{
			movement += glm::vec3{ 0.f, 0.f, -1.f };
			changed |= true;
		}
			
		if (Input::IsPressed(KeyCode::K))
		{
			movement -= glm::vec3{ 0.f, 0.f, -1.f };
			changed |= true;
		}
		if (Input::IsPressed(KeyCode::J))
		{
			movement -= glm::vec3{ 1.f, 0.f, 0.f };
			changed |= true;
		}
		if (Input::IsPressed(KeyCode::L))
		{
			movement += glm::vec3{ 1.f, 0.f, 0.f };
			changed |= true;
		}

		if (changed)
		{
			movement = glm::normalize(movement) * 3.f * iter.delta_time();

			goalEntt.get_mut<Transform>()->Position += movement;

			auto found = iter.world().lookup("MainModel");
			if (found.is_valid() && found.has<PathComponent>())
			{
				auto originPos = found.get<Transform>()->Position;
				auto endPos = goalEntt.get<Transform>()->Position;
				endPos.y = 0.f;
				auto middle1 = Math::Lerp(originPos, endPos, 0.25f);
				auto middle2 = Math::Lerp(originPos, endPos, 0.75f);
				found.set<PathComponent>({ {originPos,middle1,middle2,endPos} });
			}
		}
		

	}
}
