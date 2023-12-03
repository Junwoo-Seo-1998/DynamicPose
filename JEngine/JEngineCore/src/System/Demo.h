#pragma once
#include "System.h"
struct Config;

namespace flecs
{
	struct iter;
}

class Demo :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void DrawGUI(flecs::iter& iter, Config* config);
	//void UpdateGoal(flecs::iter& iter, EndEffect* end_effects);
};
