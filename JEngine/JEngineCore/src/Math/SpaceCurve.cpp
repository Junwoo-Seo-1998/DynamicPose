#include "SpaceCurve.h"


SpaceCurve::SpaceCurve(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	:coeff_3(-p0 + 3.f * p1 - 3.f * p2 + p3), coeff_2(3.f * p0 - 6.f * p1 + 3.f * p2), coeff_1(-3.f * p0 + 3.f * p1), coeff_0(p0)
{
	constexpr int steps = 20;
	CurveLength.reserve(steps);
	float t_step = 1.0f / static_cast<float>(steps);
	float t = 0.0f;

	float total_len = 0.0f;

	auto current = Compute(t);
	for (int i = 0; i < steps; ++i)
	{
		t += t_step;
		auto next = Compute(t);
		auto delta = glm::length(next - current);
		current = next;
		total_len += delta;
		CurveLength.push_back(total_len);
	}
}

glm::vec3 SpaceCurve::Compute(float t)
{
	const float t2 = t * t;
	const float t3 = t * t2;
	return t3 * coeff_3 + t2 * coeff_2 + t * coeff_1 + coeff_0;
}

glm::vec3 SpaceCurve::GetEndTangent()
{
	return glm::normalize(coeff_3 - coeff_2);
}

