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
private:
	void DebugRender(flecs::iter& iter, Transform* transform, DebugBone* bone);
	void RenderSkinnedMesh(flecs::iter& iter, AnimatorComponent* animator);
	void RenderSkinnedMesh(flecs::entity entity);

	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_RenderShader;
};
