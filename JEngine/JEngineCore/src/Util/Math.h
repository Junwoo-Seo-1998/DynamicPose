#pragma once
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Math/VQS.h"
#include "glm/gtc/quaternion.hpp"

class Math
{
public:
	static float GetMaxElement(const glm::vec3& val);
	static glm::mat3 GetCrossProductMatrix(const glm::vec3& vec);

	static bool Decompose(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	static float GetInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	static VQS GetInterpolation(const VQS& start, const VQS& end, float factor);

	template<typename T>
	static T Lerp(const T& start, const T& end, float factor);

	static glm::quat Slerp(const glm::quat& start, const glm::quat& end, float factor);

	static float Elerp(const float& start, const float& end, float factor);
	static glm::vec3 Elerp(const glm::vec3& start, const glm::vec3& end, float factor);

	static Mesh GenerateSpherePointsWithIndices(float radius = 1.f, int segments = 30, int rings = 30);

	static Mesh GenerateBox(const glm::vec3& size = glm::vec3{ 1.f });

	static std::tuple<glm::mat3, float> ComputeInertiaTensor(const Mesh& mesh, float expectedTotalWeight);
};

template<typename T>
inline T Math::Lerp(const T& start, const T& end, float factor)
{
	return (end - start) * factor + start;
}


