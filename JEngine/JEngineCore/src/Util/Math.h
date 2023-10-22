#pragma once
#include <glm/glm.hpp>
class Math
{
public:
	static bool Decompose(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	static float GetInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	template<typename T>
	static T Lerp(const T& start, const T& end, float factor);

	static glm::quat Slerp(const glm::quat& start, const glm::quat& end, float factor);

	static glm::vec3 Elerp(const glm::vec3& start, const glm::vec3& end, float factor);
};

template<typename T>
inline T Math::Lerp(const T& start, const T& end, float factor)
{
	return (end - start) * factor + start;
}


