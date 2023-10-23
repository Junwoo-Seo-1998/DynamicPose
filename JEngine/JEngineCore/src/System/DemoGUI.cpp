#include "DemoGUI.h"

#include <format>

#include "Components.h"
#include "flecs.h"
#include "imgui.h"

void DemoGUI::RegisterSystem(flecs::world& _world)
{
	_world.system<Config>("DEMO_GUI").kind(flecs::OnValidate).iter([&](flecs::iter& iter, Config* config)
		{
			DrawGUI(iter, config);
		});
}

void DemoGUI::DrawGUI(flecs::iter& iter, Config* config)
{
	ImGui::Begin("DemoGUI");
	for (auto i : iter)
	{
		ImGui::Text(std::format("delta time: {}", iter.delta_time()).c_str());
		ImGui::Checkbox("Show Skeleton", &config[i].ShowSkeleton);
		ImGui::Checkbox("Use VQS", &config[i].UseVQS);
		{
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
		}
	}
	ImGui::End();
}
