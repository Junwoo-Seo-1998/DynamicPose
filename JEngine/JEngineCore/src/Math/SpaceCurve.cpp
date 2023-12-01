#include "SpaceCurve.h"

#include <iostream>

#include "Util/Math.h"


SpaceCurve::SpaceCurve(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	:coeff_3(-p0 + 3.f * p1 - 3.f * p2 + p3), coeff_2(3.f * p0 - 6.f * p1 + 3.f * p2), coeff_1(-3.f * p0 + 3.f * p1), coeff_0(p0)
{
	constexpr int segments = 200;
	PreComputedPoints.reserve(segments);
	CurveLength.reserve(segments);
	InverseValues.reserve(segments);
	float t_step = 1.0f / static_cast<float>(segments);
	float t = 0.0f;

	float total_len = 0.0f;
	auto current = ComputeInner(t);

	//creating table
	for (int i = 0; i < segments-1; ++i)
	{
		auto prev = current;
		current = ComputeInner(t);
		auto delta = glm::length(current - prev);
		total_len += delta;

		PreComputedPoints.push_back(current);
		InverseValues.push_back(t);
		CurveLength.push_back(total_len);

		t += t_step;
	}

	auto prev = current;
	current = ComputeInner(1.0f);
	auto delta = glm::length(current - prev);
	total_len += delta;

	PreComputedPoints.push_back(current);
	InverseValues.push_back(1.0f);
	CurveLength.push_back(total_len);

	//normalize
	for (auto& len:CurveLength)
	{
		len /= total_len;
	}
}

glm::vec3 SpaceCurve::GetPoint(float t)
{
	int start = 0, end = InverseValues.size() - 1;

	while (start <= end)
	{
		int middle = (start + end) / 2;

		auto& toComp = InverseValues[middle];
		if (t == toComp)
		{
			return PreComputedPoints[middle];
		}
		else if (t > toComp)
		{
			start = middle + 1;
		}
		else
		{
			end = middle - 1;
		}
	}

	//couldn't find specific value
	if (start > end)
	{
		std::swap(start, end);
	}
	//interpolate
	float interpolationFactor = (t - InverseValues[start]) / (InverseValues[end] - InverseValues[start]);
	return Math::Lerp <glm::vec3> (PreComputedPoints[start], PreComputedPoints[end], interpolationFactor);
}

glm::vec3 SpaceCurve::ComputeInner(float t)
{
	const float t2 = t * t;
	const float t3 = t * t2;
	return t3 * coeff_3 + t2 * coeff_2 + t * coeff_1 + coeff_0;
}

float SpaceCurve::GetInverse(float length)
{
	//binary search
	int start = 0, end = CurveLength.size() - 1;

	while (start <= end)
	{
		int middle = (start+end) / 2;

		auto& toComp = CurveLength[middle];
		if(length==toComp)
		{
			return InverseValues[middle];
		}
		else if(length>toComp)
		{
			start = middle + 1;
		}
		else //if(length<toComp)
		{
			end = middle - 1;
		}
	}

	//if we couldn't find exact value have to find then we should interpolate
	if(start>end)
	{
		std::swap(start, end);
	}
	//interpolate
	float interpolationFactor = (length - CurveLength[start]) / (CurveLength[end] - CurveLength[start]);
	return Math::Lerp<float>(InverseValues[start], InverseValues[end], interpolationFactor);
}

std::vector<glm::vec3>& SpaceCurve::GetPreComputedPoints()
{
	return PreComputedPoints;
}

std::vector<float>& SpaceCurve::GetCurveLength()
{
	return CurveLength;
}

std::vector<float>& SpaceCurve::GetInverseValues()
{
	return CurveLength;
}

glm::vec3 SpaceCurve::GetTangent(float t)
{
	return glm::normalize(3.f * (t * t) * coeff_3 + 2.f * t * coeff_2 + coeff_1);
}
