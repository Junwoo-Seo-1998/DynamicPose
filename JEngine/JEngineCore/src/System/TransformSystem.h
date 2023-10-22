#pragma once
#include "flecs.h"
#include "System/System.h"


struct Transform;

class TransformSystem :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void UpdateTransform(flecs::iter& iter, Transform* transform, Transform* p_transform);
};
