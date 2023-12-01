#include "Math.h"
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>

float Math::GetMaxElement(const glm::vec3& val)
{
	return glm::max(glm::max(val.x, val.y), val.z);
}


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

VQS Math::GetInterpolation(const VQS& start, const VQS& end, float factor)
{
	return {
		Lerp(start.v, end.v,factor),
		Slerp(start.q, end.q, factor),
		Elerp(start.s, end.s,factor)
	};
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

float Math::Elerp(const float& start, const float& end, float factor)
{
	return glm::pow(end / start, factor) * start;
}

glm::vec3 Math::Elerp(const glm::vec3& start, const glm::vec3& end, float factor)
{
	float x = glm::pow(end.x / start.x, factor) * start.x;
	float y = glm::pow(end.y / start.y, factor) * start.y;
	float z = glm::pow(end.z / start.z, factor) * start.z;

	return { x,y,z };
}

Mesh Math::GenerateSpherePointsWithIndices(float radius, int segments, int rings)
{
	Mesh sphere;
	//clamp
	segments = std::max(segments, 3);
	rings = std::max(rings, 3);
	constexpr float pi = glm::pi<float>();

	std::vector<Vertex>& points = sphere.vertices;
	float theta_step = pi / static_cast<float>(rings);
	float p_step = 2.f * pi / static_cast<float>(segments);

	float theta = theta_step;
	for (int ring = 1; ring < rings; ++ring)
	{
		float ring_rad = radius * std::sin(theta);
		float p = 0.f;
		for (int segment = 0; segment < segments; ++segment)
		{
			points.push_back({ glm::vec3{ ring_rad * glm::sin(p),radius * std::cos(theta) , ring_rad * glm::cos(p) }, });
			p += p_step;
		}
		theta += theta_step;
	}

	std::vector<unsigned int>& indices = sphere.indices;
	int sides = rings - 2; //- top and bottom
	for (int side = 0; side < sides; ++side)
	{
		int current_circle = side * segments;
		int next_circle = (side + 1) * segments;
		for (int segment = 0; segment < segments; ++segment)
		{
			//triangle 1
			//*
			//**
			indices.push_back(current_circle + segment);
			indices.push_back(next_circle + segment);
			indices.push_back(next_circle + (segment + 1) % segments);

			//triangle 2
			//**
			// *
			indices.push_back(current_circle + segment);
			indices.push_back(next_circle + (segment + 1) % segments);
			indices.push_back(current_circle + (segment + 1) % segments);
		}
	}


	//top
	points.push_back({ glm::vec3{ 0.f,radius,0.f }, });
	int	top_index = static_cast<int>(points.size() - 1);
	for (int segment = 0; segment < segments; ++segment)
	{
		//triangle 1
		//*  top
		//** first circle
		indices.push_back(top_index);
		indices.push_back(segment % segments);
		indices.push_back((segment + 1) % segments);
	}
	//bottom
	int last_circle = sides * segments;
	points.push_back({ glm::vec3{ 0.f,-radius,0.f }, });
	int	bottom_index = static_cast<int>(points.size() - 1);
	for (int segment = 0; segment < segments; ++segment)
	{
		//triangle
		//** last_circle
		// * bottom
		indices.push_back(last_circle + segment % segments);
		indices.push_back(bottom_index);
		indices.push_back(last_circle + (segment + 1) % segments);
	}
	return sphere;
}
