#pragma once
#include <map>
#include <vector>
#include <glm/glm.hpp>

class SpaceCurve
{
public:
	SpaceCurve(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	glm::vec3 Compute(float t);

	glm::vec3 GetEndTangent();
	float Length(float t);
private:
	float ComputeCurveLength(float t);
	glm::vec3 coeff_3;
	glm::vec3 coeff_2;
	glm::vec3 coeff_1;
	glm::vec3 coeff_0;
	std::vector<glm::vec3> PreComputedPoints;
	std::vector<float>CurveLength;
};
