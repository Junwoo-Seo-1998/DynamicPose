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
#include "Math/SpaceCurve.h"
#include "Util/Math.h"

flecs::entity CreateModel(flecs::world& _world, Model& _model, const std::string& name)
{
	std::function<void(ModelNode&, flecs::entity*, glm::mat4)> helper;
	helper = [&](ModelNode& model, flecs::entity* parent, glm::mat4 ToParent)
		{
			flecs::entity obj;
			glm::vec3 t, s;
			glm::quat rot;
			Math::Decompose(model.nodeToParent, t, rot, s);

			obj = _world.entity(model.name.c_str()).child_of(*parent).set<Transform>({ t, rot,s });

			if(auto iter=_model.boneInfoMap.find(model.name); iter!=_model.boneInfoMap.end())
			{
				obj.set<BoneComponent>({ iter->second.id ,iter->second.offset });
			}

			if(model.meshes.size()>0)
			{
				for(auto m:model.meshes)
				{
					_world.entity().child_of(obj).add<Transform>().set<SkinnedMeshRenderer>({ m });
				}
			}
			else
			{
				obj.add<DebugBone>();
			}

			for (ModelNode& m : model.children)
			{
				helper(m, &obj, ToParent * model.nodeToParent);
			}
		};
	flecs::entity parent = _world.entity(name.c_str()).add<Transform>();
	helper(_model.root, &parent, _model.root.nodeToParent);
	return parent;
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
	auto animationHandle = AssimpParser::ParseAnimations("Medieval.fbx");

	auto one=CreateModel(_world, model,"MainModel");
	one.get_mut<Transform>()->Scale = { 0.01,0.01,0.01 };
	one.set<AnimatorComponent>({ animationHandle[16], });

	std::vector<glm::vec3> points =
	{
		{0.f,0.f,0.f},
		{2.f,0.f,-3.f},
		{5.f, 0.f, -2.f},
		{6.f, 0.f, 0.f},
		{5.5f, 0.f, 3.f},
		{7.f, 0.f, 4.f},
		{6.f, 0.f, 6.f},
		{4.f, 0.f, 3.f},
		{2.f, 0.f, 4.f},
	};

	one.set<PathComponent>({ points });




	/*auto two = CreateModel(_world, model, "Model_2");
	two.set<Transform>({ {100, 0, -100}, });
	two.set<AnimatorComponent>({ animationHandle[3], });*/

	_world.get_mut<Config>()->AnimationList = animationHandle;

	_world.system<Transform, DebugBone>("Debug Bone Renderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, Transform* transform, DebugBone* bone)
	{
		DebugRender(iter, transform, bone);
	});

	_world.system<PathComponent>("Debug Path Renderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, PathComponent* path)
	{
		DebugRender(iter, path);
	});

	_world.system<MeshRenderer>("MeshRenderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, MeshRenderer* mesh)
	{
		RenderMesh(iter, mesh);
	});


	_world.system<AnimatorComponent>("SkinnedMeshRenderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, AnimatorComponent* animator)
	{
		RenderSkinnedMesh(iter, animator);
	});

	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	
}

void SceneRenderer::DebugRender(flecs::iter& iter, Transform* transform, DebugBone* bone)
{
	if(!iter.world().get<Config>()->ShowSkeleton)
		return;
	glDisable(GL_DEPTH_TEST);
	DebugRenderer::BeginScene(Application::Get().GetWorld().get<MainCamera>()->projection
		* Application::Get().GetWorld().get<MainCamera>()->view, { 0.f,1.f,0.f });
	for(auto i: iter)
	{
		auto current = iter.entity(i);
		if (current.parent().is_valid() && current.parent().has<DebugBone>())
		{
			glm::vec3 p1 = current.parent().get<Transform>()->FinalTransformMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
			glm::vec3 p2 = transform[i].FinalTransformMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
			DebugRenderer::DrawLine(p1, p2);
		}
	}
	DebugRenderer::EndScene();
}

void SceneRenderer::DebugRender(flecs::iter& iter, PathComponent* path)
{
	glDisable(GL_DEPTH_TEST);
	DebugRenderer::BeginScene(Application::Get().GetWorld().get<MainCamera>()->projection
		* Application::Get().GetWorld().get<MainCamera>()->view, { 1.f,1.f,0.f });
	for (auto i : iter)
	{
		for (auto& curve: path[i].Curves)
		{
			float step = 30.f;
			float t_inc = 1.f / step;

			glm::vec3 p1 = curve.GetPoint(0.f);
			for (float t = t_inc; t < 1.f; t += t_inc)
			{
				auto p2 = curve.GetPoint(t);
				DebugRenderer::DrawLine(p1, p2);
				p1 = p2;
			}
			DebugRenderer::DrawLine(p1, curve.GetPoint(1.f));
		}
	}
	DebugRenderer::EndScene();


	DebugRenderer::BeginScene(Application::Get().GetWorld().get<MainCamera>()->projection
		* Application::Get().GetWorld().get<MainCamera>()->view, { 0.5f,0.0f,0.0f });
	for (auto i : iter)
	{
		auto& controlPoints = path[i].controlPoints;
		int numOfPoints = static_cast<int>(controlPoints.size());
		if (numOfPoints <= 1)
			continue;
		glm::vec3 p1 = controlPoints[0];
		for (int p_index = 1; p_index < numOfPoints; ++p_index)
		{
			glm::vec3 p2 = controlPoints[p_index];
			DebugRenderer::DrawLine(p1, p2);
			p1 = p2;
		}
	}
	DebugRenderer::EndScene();
}


void SceneRenderer::RenderMesh(flecs::iter& iter, MeshRenderer* mesh)
{
	glEnable(GL_DEPTH_TEST);
}

void SceneRenderer::RenderSkinnedMesh(flecs::iter& iter, AnimatorComponent* animator)
{
	glEnable(GL_DEPTH_TEST);
	m_VertexArray->Bind();
	m_RenderShader->Use();
	m_RenderShader->SetFloat3("CamPos", Application::Get().GetWorld().get<MainCamera>()->position);
	m_RenderShader->SetMat4("Matrix.View", Application::Get().GetWorld().get<MainCamera>()->view);
	m_RenderShader->SetMat4("Matrix.Projection", Application::Get().GetWorld().get<MainCamera>()->projection);
	m_RenderShader->SetFloat4("Color", glm::vec4{ 1.f });

	for (auto i:iter)
	{
		auto rootEntity = iter.entity(i);
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
