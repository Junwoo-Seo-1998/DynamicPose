#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Animation.h"
#include "Math/VQS.h"

using quaternion = glm::quat;
struct Transform
{
	glm::vec3 Position{ 0.f,0.f,0.f };
	quaternion Rotation{ 1.f, 0.f,0.f,0.f };
	glm::vec3 Scale{ 1.f,1.f,1.f };

	VQS FinalVQS;
	glm::mat4 FinalTransformMatrix{ 1.f };

	glm::vec3 GetRight() const
	{
		return glm::toMat4(Rotation) * glm::vec4{ 1.f,0.f,0.f ,0.f };
	}
	glm::vec3 GetUp() const
	{
		return glm::toMat4(Rotation) * glm::vec4{ 0.f,1.f,0.f ,0.f };
	}
	glm::vec3 GetForward() const
	{
		return glm::toMat4(Rotation) * glm::vec4{ 0.f,0.f,-1.f ,0.f };
	}
};

struct DebugBone
{
	glm::mat4 transform;
};

struct SkinnedMeshRenderer
{
	MeshInstance instance;
};

struct MainCamera
{
	glm::vec3 position{ 0.f,0.f,0.f };
	glm::mat4 view = glm::identity<glm::mat4>();
	glm::mat4 projection = glm::identity<glm::mat4>();
};
struct Camera
{};

struct AnimatorComponent
{
	std::shared_ptr<Animation> CurrentAnimation;
	std::vector<glm::mat4> FinalBoneMatrices{ 100, glm::mat4(1.f) };
	float CurrentTime = 0.f;
};

struct BoneComponent
{
	int BoneMatrixID = -1;
	glm::mat4 BoneOffset = glm::mat4(1.f);
};
