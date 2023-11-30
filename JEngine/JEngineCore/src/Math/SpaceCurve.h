#pragma once
#include <map>
#include <vector>
#include <glm/glm.hpp>

class SpaceCurve
{
public:
	SpaceCurve(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	glm::vec3 GetPoint(float t);
	float GetInverse(float length);
	std::vector<glm::vec3>& GetPreComputedPoints();
	std::vector<float>& GetCurveLength();
	std::vector<float>& GetInverseValues();

	glm::vec3 GetTangent(float t);
	
private:
	glm::vec3 ComputeInner(float t);

	glm::vec3 coeff_3;
	glm::vec3 coeff_2;
	glm::vec3 coeff_1;
	glm::vec3 coeff_0;

	std::vector<glm::vec3> PreComputedPoints;
	std::vector<float>CurveLength;
	std::vector<float>InverseValues;
};
