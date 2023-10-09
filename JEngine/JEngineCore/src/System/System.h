#pragma once
namespace flecs
{
	struct world;
}

class System
{
public:
	virtual void RegisterSystem(flecs::world& _world) = 0;
};
