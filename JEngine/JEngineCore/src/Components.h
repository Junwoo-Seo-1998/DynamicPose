#pragma once

#include "Mesh.h"
#include"glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/quaternion.hpp>
using quaternion = glm::quat;
struct Transform
{
	glm::vec3 position{ 0.f,0.f,0.f };
	quaternion rotation{ 1.f, 0.f,0.f,0.f };
	glm::vec3 scale{ 1.f,1.f,1.f };

	glm::vec3 GetRight() const
	{
		return glm::toMat4(rotation) * glm::vec4{ 1.f,0.f,0.f ,0.f };
	}
	glm::vec3 GetUp() const
	{
		return glm::toMat4(rotation) * glm::vec4{ 0.f,1.f,0.f ,0.f };
	}
	glm::vec3 GetForward() const
	{
		return glm::toMat4(rotation) * glm::vec4{ 0.f,0.f,-1.f ,0.f };
	}
};

struct DebugBone
{
	glm::mat4 transform;
};

struct Renderer
{
	MeshInstance instance;
};

struct MainCamera
{
	glm::mat4 view = glm::identity<glm::mat4>();
};
struct Camera
{};
