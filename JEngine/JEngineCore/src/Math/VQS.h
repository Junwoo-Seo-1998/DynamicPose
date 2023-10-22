#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
struct VQS
{
	glm::vec3 v;
	glm::quat q;
	//uniform scale
	float s;
	
	glm::vec3 operator*(const glm::vec3& right);
	VQS operator*(const VQS& right);

	friend VQS operator*(const float left, const VQS& right);

	glm::mat4 toMat() const;
};

VQS operator*(const float left, const VQS& right);
