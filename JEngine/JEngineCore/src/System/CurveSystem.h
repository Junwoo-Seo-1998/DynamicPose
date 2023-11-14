#pragma once
#include "flecs.h"
#include "System/System.h"


struct PathComponent;

class CurveSystem :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void OnChange(PathComponent& _path);

	void Update(flecs::iter& iter, PathComponent* path);
};
