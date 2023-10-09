#pragma once
#include "flecs.h"
#include "System/System.h"
#include "Components.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"

class SceneRenderer:public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void RenderScene(flecs::iter& iter, Transform* transform, Renderer* renderer);
private:
	void DebugRender(flecs::iter& iter, DebugBone* bone);

	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_RenderShader;
};
