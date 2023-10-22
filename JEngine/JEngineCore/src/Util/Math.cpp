#include "Math.h"
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
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
