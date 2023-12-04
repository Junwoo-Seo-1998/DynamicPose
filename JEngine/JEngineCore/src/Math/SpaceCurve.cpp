#include "SpaceCurve.h"

#include <iostream>
#include <list>

#include "Util/Math.h"


SpaceCurve::SpaceCurve(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	:coeff_3(-p0 + 3.f * p1 - 3.f * p2 + p3), coeff_2(3.f * p0 - 6.f * p1 + 3.f * p2), coeff_1(-3.f * p0 + 3.f * p1), coeff_0(p0)
{
	constexpr int segments = 200;

	std::vector<glm::vec3> tempPreComputed;
	std::vector<float>tempArcLength;
	std::vector<float>tempUValues;

	tempPreComputed.reserve(segments);
	tempArcLength.reserve(segments);
	tempUValues.reserve(segments);
	float t_step = 1.0f / static_cast<float>(segments);
	float t = 0.0f;

	float total_len = 0.0f;
	auto current = ComputeInner(t);

	//creating temp table
	for (int i = 0; i < segments-1; ++i)
	{
		auto prev = current;
		current = ComputeInner(t);
		auto delta = glm::length(current - prev);
		total_len += delta;

		tempPreComputed.push_back(current);
		tempUValues.push_back(t);
		tempArcLength.push_back(total_len);

		t += t_step;
	}

	auto prev = current;
	current = ComputeInner(1.0f);
	auto delta = glm::length(current - prev);
	total_len += delta;

	tempPreComputed.push_back(current);
	tempUValues.push_back(1.0f);
	tempArcLength.push_back(total_len);
	PreComputedPoints = tempPreComputed;
	UValues = tempUValues;

	//adaptive approach
	constexpr bool useAdaptive = true;
	if (useAdaptive)
	{
		float errorThreshold = 0.0001f;
		std::list<std::tuple<float, float>> seglist;
		seglist.push_back({ 0.f,1.f });
		InverseValues.push_back(0.f);
		CurveLength.push_back(0.f);
		while (!seglist.empty())
		{
			auto [ua, ub] = seglist.front();
			seglist.pop_front();
			float um = (ua + ub) / 2.f;
			glm::vec3 pa = ComputeInner(ua);
			glm::vec3 pb = ComputeInner(ub);
			glm::vec3 pm = ComputeInner(um);
			float A = glm::length(pa - pm);
			float B = glm::length(pm - pb);
			float C = glm::length(pa - pb);
			float d = A + B - C;
			if (d > errorThreshold)
			{
				seglist.push_front({ um, ub });
				seglist.push_front({ ua, um });
			}
			else
			{
				//record
				InverseValues.push_back(um);
				InverseValues.push_back(ub);

				float Gu_a = GetArcLenHelper(tempUValues, tempArcLength, ua);
				float Gu_m = GetArcLenHelper(tempUValues, tempArcLength, um);

				CurveLength.push_back(Gu_a + A);
				CurveLength.push_back(Gu_m + B);
			}
		}
	}
	else
	{
		InverseValues = tempUValues;
		CurveLength = tempArcLength;
	}

	//normalize
	total_len = CurveLength[CurveLength.size() - 1];
	for (auto& len: CurveLength)
	{
		len /= total_len;
	}
	
}

glm::vec3 SpaceCurve::GetPoint(float t)
{
	int start = 0, end = static_cast<int>(UValues.size()) - 1;

	while (start <= end)
	{
		int middle = (start + end) / 2;

		auto& toComp = UValues[middle];
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
	float interpolationFactor = (t - UValues[start]) / (UValues[end] - UValues[start]);
	return Math::Lerp <glm::vec3> (PreComputedPoints[start], PreComputedPoints[end], interpolationFactor);
}

glm::vec3 SpaceCurve::ComputeInner(float t)
{
	const float t2 = t * t;
	const float t3 = t * t2;
	return t3 * coeff_3 + t2 * coeff_2 + t * coeff_1 + coeff_0;
}

float SpaceCurve::GetArcLenHelper(std::vector<float> uVals, std::vector<float>& arcLen, float t)
{
	//binary search
	int start = 0, end = static_cast<int>(uVals.size()) - 1;

	while (start <= end)
	{
		int middle = (start + end) / 2;

		auto& toComp = uVals[middle];
		if (t == toComp)
		{
			return arcLen[middle];
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

	//if we couldn't find exact value have to find then we should interpolate
	if (start > end)
	{
		std::swap(start, end);
	}
	//interpolate
	float interpolationFactor = (t - uVals[start]) / (uVals[end] - uVals[start]);
	return Math::Lerp<float>(arcLen[start], arcLen[end], interpolationFactor);
}


float SpaceCurve::GetInverse(float length)
{
	//binary search
	int start = 0, end = static_cast<int>(CurveLength.size()) - 1;

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

std::vector<float>& SpaceCurve::GetUValues()
{
	return UValues;
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
