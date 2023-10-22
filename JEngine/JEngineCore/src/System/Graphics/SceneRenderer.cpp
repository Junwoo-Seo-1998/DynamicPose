#include "SceneRenderer.h"

#include <functional>

#include "flecs.h"
#include "Components.h"
#include "Parser.h"
#include <iostream>

#include "Graphics/MeshFactory.h"
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>
#include "Application.h"
#include "Graphics/DebugRenderer.h"
#include "Util/Math.h"

void CreateModel(flecs::world& _world, Model& _model)
{
	std::function<void(ModelNode&, flecs::entity*, glm::mat4)> helper;
	helper = [&](ModelNode& model, flecs::entity* parent, glm::mat4 ToParent)
		{
			flecs::entity obj;
			glm::vec3 t, s;
			glm::quat rot;
			Math::Decompose(model.nodeToParent, t, rot, s);

			obj = _world.entity(model.name.c_str()).child_of(*parent).set<Transform>({ t, rot,s });
			std::cout << model.name << std::endl;
			std::cout << glm::to_string(t) << std::endl;

			if(auto iter=_model.boneInfoMap.find(model.name); iter!=_model.boneInfoMap.end())
			{
				obj.set<BoneComponent>({ iter->second.id ,iter->second.offset });
			}

			if(model.meshes.size()>0)
			{
				std::cout << "mesh: " << model.name << std::endl;
				for(auto m:model.meshes)
				{
					_world.entity().child_of(obj).add<Transform>().set<SkinnedMeshRenderer>({ m });
				}
			}
			else
			{
				obj.set<DebugBone>({ ToParent * model.nodeToParent });
			}

			for (ModelNode& m : model.children)
			{
				helper(m, &obj, ToParent * model.nodeToParent);
			}
		};
	flecs::entity parent = _world.entity(_model.name.c_str()).add<Transform>();
	/*if (_model.boneInfoMap.contains(_model.name))
	{
		parent.set<DebugBone>({ _model.root.nodeToParent });
	}*/
	std::cout << glm::to_string(_model.root.nodeToParent) << std::endl;
	helper(_model.root, &parent, _model.root.nodeToParent);
}

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

	auto animationHandle = AssimpParser::ParseAnimation("Medieval.fbx");
	_world.lookup("Medieval").set<AnimatorComponent>({ animationHandle, });

	_world.system<AnimatorComponent>("SkinnedMeshRenderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, AnimatorComponent* animator)
		{
			//RenderScene(iter, transform, renderer);
			RenderSkinnedMesh(iter, animator);
		});

	_world.system<Transform, DebugBone>("Debug Bone Renderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, Transform* transform, DebugBone* bone)
		{
			DebugRender(iter, transform, bone);
		});

	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	
}

void SceneRenderer::DebugRender(flecs::iter& iter, Transform* transform, DebugBone* bone)
{
	glDisable(GL_DEPTH_TEST);
	DebugRenderer::BeginScene(glm::perspective(glm::radians(90.f), 1.0f, 0.01f, 1000.f)
		* Application::Get().GetWorld().get<MainCamera>()->view, { 0.f,1.f,0.f });
	for(int i: iter)
	{
		auto current = iter.entity(i);
		if (current.parent().is_valid() && current.parent().has<DebugBone>())
		{
			glm::vec3 p1 = current.parent().get<Transform>()->final * glm::vec4(0.f, 0.f, 0.f, 1.f);
			glm::vec3 p2 = transform[i].final * glm::vec4(0.f, 0.f, 0.f, 1.f);
			DebugRenderer::DrawLine(p1, p2);
		}
	}
	DebugRenderer::EndScene();
}

void SceneRenderer::RenderSkinnedMesh(flecs::iter& iter, AnimatorComponent* animator)
{
	glEnable(GL_DEPTH_TEST);
	m_VertexArray->Bind();
	m_RenderShader->Use();
	m_RenderShader->SetFloat3("CamPos", Application::Get().GetWorld().get<MainCamera>()->position);
	m_RenderShader->SetMat4("Matrix.View", Application::Get().GetWorld().get<MainCamera>()->view);
	glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.0f, 0.001f, 1000.f);
	m_RenderShader->SetMat4("Matrix.Projection", projection);
	m_RenderShader->SetFloat4("Color", glm::vec4{ 1.f });

	

	for (int i:iter)
	{
		auto rootEntity = iter.entity(i);
		m_RenderShader->SetMat4("Matrix.Model", rootEntity.get<Transform>()->final);

		AnimatorComponent& animator_component = animator[i];
		auto& BoneMatrices = animator_component.FinalBoneMatrices;
		int size = static_cast<int>(BoneMatrices.size());
		for (int matIndex = 0; matIndex < size; ++matIndex)
			m_RenderShader->SetMat4("finalBonesMatrices[" + std::to_string(matIndex) + "]", BoneMatrices[matIndex]);

		RenderSkinnedMesh(rootEntity);
	}
}

void SceneRenderer::RenderSkinnedMesh(flecs::entity entity)
{
	if(entity.has<SkinnedMeshRenderer>())
	{
		const SkinnedMeshRenderer* renderer = entity.get<SkinnedMeshRenderer>();
		renderer->instance.m_Buffer->BindToVertexArray();
		renderer->instance.m_IndexBuffer->BindToVertexArray();
		glDrawElements(GL_TRIANGLES, renderer->instance.m_IndexBuffer->GetSize(), GL_UNSIGNED_INT, nullptr);
	}
	entity.children([&](flecs::entity child)
	{
		RenderSkinnedMesh(child);
	});
}
