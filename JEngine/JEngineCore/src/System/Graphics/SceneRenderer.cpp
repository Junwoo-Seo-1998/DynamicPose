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
#include "Util/EntityUtil.h"
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
					if(m.skinned)
						_world.entity().child_of(obj).add<Transform>().set<SkinnedMeshRenderer>({ m });
					else
						_world.entity().child_of(obj).add<Transform>().set<MeshRenderer>({ m });
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
	{
		ShaderSource source{};
		source[ShaderType::VertexShader] = { "Shader/simple.vert" };
		source[ShaderType::FragmentShader] = { "Shader/simple.frag" };
		m_RenderShader = Shader::CreateShaderFromFile(source);
	}

	{
		ShaderSource source{};
		source[ShaderType::VertexShader] = { "Shader/simpleMesh.vert" };
		source[ShaderType::FragmentShader] = { "Shader/simpleMesh.frag" };
		m_MeshRenderShader = Shader::CreateShaderFromFile(source);
	}

	/*Model plane=AssimpParser::ParseModel("Plane.fbx");
	auto planeEntity  = CreateModel(_world, plane, "PlaneObj");
	planeEntity.get_mut<Transform>()->Scale = { 0.1f,0.1f,0.1f };*/

	Model model = AssimpParser::ParseModel("Medieval.fbx");
	auto animationHandle = AssimpParser::ParseAnimations("Medieval.fbx");

	auto one = CreateModel(_world, model, "MainModel");
	one.get_mut<Transform>()->Position = { -3.f, 0.f, 0.f };
	one.get_mut<Transform>()->Scale = { 0.01f,0.01f,0.01f };
	one.set<AnimatorComponent>({ animationHandle[16], true });

	auto fixed = _world.entity("fixed_left")
		.set<Transform>({ {0.f, 5.f, 0.f}, })
		.set<DebugSphere>({ 0.3f, {1.f, 0.f,0.f} });

	Mesh box = Math::GenerateBox(glm::vec3{ 2.f, 1.f, 1.f });

	auto [I_Obj, boxWeight] = Math::ComputeInertiaTensor(box, 1.f);
	RigidBody rigid_body{};
	rigid_body.InverseMass = 1.f / boxWeight;
	rigid_body.OriginalInertiaTensor = I_Obj;
	rigid_body.OriginalInverseInertiaTensor = glm::inverse(I_Obj);
	rigid_body.InverseInertiaTensor = rigid_body.OriginalInverseInertiaTensor;
	//rigid_body.TorqueAccumulated += glm::cross(glm::vec3{ 1.f,0.f,0.f }, glm::vec3{ 0.f, 30.f, -30.f });

	auto test = _world.entity("test")
		.set<Transform>({ {0.f, 4.f, 0.f}, });
		//.set<DebugSphere>({ 0.3f, {1.f, 0.f,0.f} });
		//test.add<RigidBody>();
	test.set<MeshRenderer>({ box });
	test.set<RigidBody>(rigid_body);




	/*std::vector<glm::vec3> points =
	{
		/#1#*{1.f,0.f,0.f},
		{2.f,0.f,0.0f},
		{3.f,0.f,0.0f},
		{4.f,0.f,0.f},
		{5.f,0.f,0.f},#2#
		{0.f,0.f,0.f},
		{2.f,0.f,-3.f},
		{5.f, 0.f, -2.f},
		{6.f, 0.f, 0.f},
		{5.5f, 0.f, 3.f},
		{7.f, 0.f, 4.f},
		{6.f, 0.f, 6.f},
		{4.f, 0.f, 5.f},
		{2.f, 0.f, 4.f},#1#
	};

	auto sphere = Math::GenerateSpherePointsWithIndices();


	//set ik joints
	auto goal = _world.entity("Goal").add<Transform>().add<IKGoal>();
	goal.set<Transform>({ {0.3f,1.65f,0.5f} });

	//set ik joints
	//this is prioritized joint(representation of flexibility of the joints)
	std::vector<std::string> joints
	{
		//priority of joints
		"LowerArm.L",
		"UpperArm.L",
		"Shoulder.L",
		"Chest",
		"Wrist.L",
		"Torso",
	};

	std::vector<IKJointComponent> constrains
	{
		//[a_min, a_max] [b_min, b_max] //optional (z) [c_min, c_max]
		//LowerArm
		{{0.f, 90.f, 0.f, 0.f}},
		//UpperArm
		{{ 0.f, 90.f, -10.f, 90.f }},
		//Shoulder 
		{{ -10.f, 10.f, -10.f, 10.f }},
		//chest
		{{ -15.f, 15.f, -30.f, 30.f , 5.f, 5.f}, true, true, true},
		
		//Wrist
		{{ -90.f, 90.f, -180.f, 180.f, -5.f, 5.f }, true,true, true},

		//torso
		{{-0.f, 5.f, -5.f, 5.f, 0.1f, 0.1f}, true, true, true},
	};
	
	std::vector<uint64_t> jointIDs;

	for (int i=0; i<joints.size(); ++i)
	{
		auto joint = EntityUtil::FindChildWithName(joints[i], one);
		joint.set<IKJointComponent>(constrains[i]);
		jointIDs.push_back(joint.id());
	}

	auto endpointName = "Index4.L";
	auto IKee = EntityUtil::FindChildWithName(endpointName, one);
	IKee.set<IKEndEffectComponent>({ goal.id() });
	one.set<IKComponent>({ jointIDs, IKee.id() });

	one.set<PathComponent>({ points });*/

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

	

	_world.system<IKJointComponent>("IK Renderer").kind(flecs::OnValidate).iter([&](flecs::iter& iter, IKJointComponent* joints)
	{

		DebugRender(iter, joints);
	});

	_world.system<DebugSphere>("Debug Sphere").kind(flecs::OnValidate).iter([&](flecs::iter& iter, DebugSphere* sphere)
		{
			glEnable(GL_DEPTH_TEST);
			glm::mat4 viewProj = Application::Get().GetWorld().get<MainCamera>()->projection
				* Application::Get().GetWorld().get<MainCamera>()->view;

			DebugRenderer::SetViewProjection(viewProj);
			for (auto i : iter)
			{
				DebugRenderer::DrawSphere(iter.entity(i).get<Transform>()->GetWorldOrigin(), sphere[i].rad, sphere[i].color);
			}
		});

	_world.system<IKGoal>("IK Goal").kind(flecs::OnValidate).iter([&](flecs::iter& iter, IKGoal* goal)
		{
			glEnable(GL_DEPTH_TEST);
			glm::mat4 viewProj = Application::Get().GetWorld().get<MainCamera>()->projection
				* Application::Get().GetWorld().get<MainCamera>()->view;

			DebugRenderer::SetViewProjection(viewProj);
			for (auto i : iter)
			{
				DebugRenderer::DrawSphere(iter.entity(i).get<Transform>()->GetWorldOrigin(), 0.07f, { 0.4f,0.f,0.4f });
			}
		});

	_world.system<IKEndEffectComponent>("IKEndEffect").kind(flecs::OnValidate).iter([&](flecs::iter& iter, IKEndEffectComponent* goal)
		{
			glEnable(GL_DEPTH_TEST);
			glm::mat4 viewProj = Application::Get().GetWorld().get<MainCamera>()->projection
				* Application::Get().GetWorld().get<MainCamera>()->view;

			DebugRenderer::SetViewProjection(viewProj);
			for (auto i : iter)
			{
				DebugRenderer::DrawSphere(iter.entity(i).get<Transform>()->GetWorldOrigin(), 0.07f, { 0.f,0.f,0.8f });
			}
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
	DebugRenderer::BeginDrawLine(Application::Get().GetWorld().get<MainCamera>()->projection
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
	DebugRenderer::EndDrawLine();
}

void SceneRenderer::DebugRender(flecs::iter& iter, PathComponent* path)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 viewProj = Application::Get().GetWorld().get<MainCamera>()->projection
		* Application::Get().GetWorld().get<MainCamera>()->view;
	DebugRenderer::BeginDrawLine(viewProj, { 1.f,1.f,0.f });
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
	DebugRenderer::EndDrawLine();

	DebugRenderer::SetViewProjection(viewProj);
	for (auto i : iter)
	{
		auto& controlPoints = path[i].controlPoints;
		for (auto& p:controlPoints)
		{
			DebugRenderer::DrawSphere(p, 0.1f, { 0.3f,1.f,0.3f });
		}
	}

	/*DebugRenderer::BeginDrawLine(Application::Get().GetWorld().get<MainCamera>()->projection
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
	DebugRenderer::EndDrawLine();*/
}

void SceneRenderer::DebugRender(flecs::iter& iter, IKJointComponent* joints)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 viewProj = Application::Get().GetWorld().get<MainCamera>()->projection
		* Application::Get().GetWorld().get<MainCamera>()->view;

	DebugRenderer::SetViewProjection(viewProj);

	for (auto i : iter)
	{
		auto entity = iter.entity(i);
		auto transform = entity.get<Transform>();
		DebugRenderer::DrawSphere(transform->GetWorldOrigin(), 0.03f, {1.f,0.f,0.f});
	}
}


void SceneRenderer::RenderMesh(flecs::iter& iter, MeshRenderer* mesh)
{
	glEnable(GL_DEPTH_TEST);
	m_VertexArray->Bind();
	m_MeshRenderShader->Use();
	m_MeshRenderShader->SetFloat3("CamPos", Application::Get().GetWorld().get<MainCamera>()->position);
	m_MeshRenderShader->SetMat4("Matrix.View", Application::Get().GetWorld().get<MainCamera>()->view);
	m_MeshRenderShader->SetMat4("Matrix.Projection", Application::Get().GetWorld().get<MainCamera>()->projection);
	m_MeshRenderShader->SetFloat4("Color", glm::vec4{ 1.f });

	for (auto i : iter)
	{
		auto entity = iter.entity(i);
		m_MeshRenderShader->SetMat4("Matrix.Model", entity.get<Transform>()->FinalTransformMatrix);
		MeshRenderer& renderer = mesh[i];
		renderer.instance.m_Buffer->BindToVertexArray();
		renderer.instance.m_IndexBuffer->BindToVertexArray();
		glDrawElements(GL_TRIANGLES, renderer.instance.m_IndexBuffer->GetSize(), GL_UNSIGNED_INT, nullptr);
	}
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
