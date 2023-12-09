#pragma once
#include "Components.h"
#include "flecs.h"
#include "System/System.h"

class PhysicsSystem : public System
{
public:
	void RegisterSystem(flecs::world& _world) override;
public:
	void Update(flecs::iter& iter, RigidBody* bodies);

	void UpdateSpringPhysics(flecs::iter& iter, SpringJointComponent* springComponents);
};

