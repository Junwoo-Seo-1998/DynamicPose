#pragma once
#include "flecs.h"
#include "System/System.h"


class SceneViewer :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void RenderSceneGUI(flecs::iter iter);
};
