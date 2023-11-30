#include "DistanceTime.h"
#include <numbers>
#include <cmath>
SineInterpolation::SineInterpolation(float t1, float t2)
	:t1(t1), t2(t2)
{
	constexpr float pi_div2_min1 = (std::numbers::pi_v<float> / 2.f) - 1.f;
	L = pi_div2_min1 * (t2 - t1) + 1.f;
}

float SineInterpolation::GetDistance(float time)
{
	constexpr float pi = std::numbers::pi_v<float>;
	if(0.0f<=time && time <=t1)
	{
		const float t1_divL = t1 / L;
		return t1_divL * std::sinf((pi / 2.f) * ((time - t1) / t1)) + t1_divL;
	}
	else if(t1<time && time <t2)
	{
		const float t1_divL = t1 / L;
		return pi / (2.f * L) * (time - t1) + t1_divL;
	}

	const float one_sub_t2_divL = (1.f - t2) / L;
	const float t2_sub_one_divL = (t2 - 1.f) / L;
	const float inner = (pi / 2.f) * ((time - t2) / (1.f - t2));
	return one_sub_t2_divL * std::sinf(inner) + t2_sub_one_divL + 1.f;
}

Parabolic::Parabolic(float t1, float t2)
	:v(2.f/(1.f-t1+t2)), t1(t1), t2(t2)
{
}

float Parabolic::GetDistance(float time)
{
	if (0.0f <= time && time <= t1)
	{
		return (v / (2.f * t1)) * (time * time);
	}
	else if (t1 < time && time < t2)
	{
		return v * (time - (t1 / 2.f));
	}

	const float left = (v * (time - t2)) / (2.f * (1.f - t2)) * (2.f - time - t2);
	const float right = v * (t2 - (t1 / 2.f));
	return left + right;
}
