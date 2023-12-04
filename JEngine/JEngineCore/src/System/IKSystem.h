#pragma once
#include "flecs.h"
#include "System/System.h"
struct IKComponent;

class IKSystem :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;
private:
	void UpdateIK(flecs::iter& iter, IKComponent* iks);
};
