#include "Math.h"
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
bool Math::Decompose(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
{
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	bool result = glm::decompose(transform, scale, rot, translation, skew, perspective);
	rotation = rot;
	return result;
}

float Math::GetInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff;
}

glm::quat Math::Slerp(const glm::quat& start, const glm::quat& end, float factor)
{
	auto z = end;

	float cosTheta = glm::dot(start, end);

	// If cosTheta < 0, the interpolation will take the long way around the sphere.
	// To fix this, one quat must be negated.
	if (cosTheta < 0.f)
	{
		z = -end;
		cosTheta = -cosTheta;
	}

	// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
	if (cosTheta > 1.f - glm::epsilon<float>())
	{
		// Linear interpolation
		return Lerp(start, end, factor);
	}

	float angle = glm::acos(cosTheta);
	return (glm::sin((1.f - factor) * angle) * start + glm::sin(factor * angle) * z) / glm::sin(angle);
}

glm::vec3 Math::Elerp(const glm::vec3& start, const glm::vec3& end, float factor)
{
	float x = glm::pow(end.x / start.x, factor) * start.x;
	float y = glm::pow(end.y / start.y, factor) * start.y;
	float z = glm::pow(end.z / start.z, factor) * start.z;

	return { x,y,z };
}
