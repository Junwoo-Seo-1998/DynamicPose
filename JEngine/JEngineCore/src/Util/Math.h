#pragma once
#include <glm/glm.hpp>
class Math
{
public:
	static bool Decompose(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	static float GetInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
};