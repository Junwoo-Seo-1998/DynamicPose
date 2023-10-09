#pragma once
#include "flecs.h"
#include "System/System.h"
#include "Components.h"

class SceneCamera :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void UpdateCamera(flecs::iter& iter, Transform* transform);
};
