#include "SceneRenderer.h"

#include <functional>

#include "flecs.h"
#include "Components.h"
#include "Parser.h"
#include <iostream>

#include "Graphics/MeshFactory.h"
#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

#include "Animator.h"
#include "Application.h"
#include "Graphics/DebugRenderer.h"

void CreateModel(flecs::world& _world, Model& _model)
{
	std::function<void(ModelNode&, flecs::entity*, glm::mat4)> helper;
	helper = [&](ModelNode& model, flecs::entity* parent, glm::mat4 ToParent)
		{
			flecs::entity obj;
			obj = _world.entity(model.name.c_str()).child_of(*parent).add<Transform>();

			if(_model.boneInfoMap.contains(model.name))
			{
				obj.set<DebugBone>({ ToParent * model.nodeToParent });
			}

			if(model.meshes.size()>0)
			{
				for(auto m:model.meshes)
				{
					_world.entity().add<Transform>().set<Renderer>({m});
				}
			}

			for (ModelNode& m : model.children)
			{
				helper(m, &obj, ToParent * model.nodeToParent);
			}
		};
	flecs::entity parent = _world.entity(_model.name.c_str()).add<Transform>();
	helper(_model.root, &parent, glm::inverse(_model.root.nodeToParent));
}
Animation ani;
std::shared_ptr<Animator> animator;
std::vector<glm::mat4> finalMat;
void SceneRenderer::RegisterSystem(flecs::world& _world)
{
	m_VertexArray = VertexArray::CreateVertexArray();
	//testing
	ShaderSource source{};
	source[ShaderType::VertexShader] = { "Shader/simple.vert" };
	source[ShaderType::FragmentShader] = {"Shader/simple.frag"};
	m_RenderShader = Shader::CreateShaderFromFile(source);

	Model model = AssimpParser::ParseModel("Medieval.fbx");
	CreateModel(_world, model);

	ani = AssimpParser::ParseAnimation("Medieval.fbx", model);
	animator = std::make_shared<Animator>(&ani);
	

	_world.system<Transform, Renderer>("SceneRenderer").iter([&](flecs::iter& iter, Transform* transform, Renderer* renderer)
		{
			RenderScene(iter, transform, renderer);
		});

	_world.system<DebugBone>("Debug Bone Renderer").kind(flecs::OnUpdate).iter([&](flecs::iter& iter, DebugBone* bone)
		{
			DebugRender(iter, bone);
		});
}

void SceneRenderer::RenderScene(flecs::iter& iter, Transform* transform, Renderer* renderer)
{
	m_VertexArray->Bind();

	m_RenderShader->Use();

	animator->UpdateAnimation(iter.delta_time());
	finalMat = animator->GetFinalBoneMatrices();

	for (int i = 0; i < finalMat.size(); ++i)
		m_RenderShader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalMat[i]);

	m_RenderShader->SetMat4("Matrix.View", Application::Get().GetWorld().get<MainCamera>()->view);


	glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.0f, 0.001f, 1000.f);
	m_RenderShader->SetMat4("Matrix.Projection", projection);
	m_RenderShader->SetFloat4("Color", glm::vec4{ 1.f });

	for (int i: iter)
	{
		m_RenderShader->SetMat4("Matrix.Model", glm::translate(glm::mat4(1.0f), transform->position)
			* glm::toMat4(transform->rotation)
			* glm::scale(glm::mat4(1.0f), transform[i].scale));

		renderer[i].instance.m_Buffer->BindToVertexArray();
		renderer[i].instance.m_IndexBuffer->BindToVertexArray();
		glDrawElements(GL_TRIANGLES, renderer[i].instance.m_IndexBuffer->GetSize(), GL_UNSIGNED_INT, nullptr);
	}
}

void SceneRenderer::DebugRender(flecs::iter& iter, DebugBone* bone)
{
	DebugRenderer::BeginScene(glm::perspective(glm::radians(90.f), 1.0f, 0.001f, 1000.f)
		* Application::Get().GetWorld().get<MainCamera>()->view, { 0.f,1.f,0.f });
	for(int i: iter)
	{
		auto current = iter.entity(i);
		if (current.parent().is_valid() && current.parent().has<DebugBone>())
		{
			glm::vec3 p1 = current.parent().get<DebugBone>()->transform * glm::vec4(0.f, 0.f, 0.f, 1.f);
			glm::vec3 p2 = bone[i].transform * glm::vec4(0.f, 0.f, 0.f, 1.f);
			DebugRenderer::DrawLine(p1, p2);
		}
	}
	DebugRenderer::EndScene();
}
