#include "Math.h"
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

float Math::GetMaxElement(const glm::vec3& val)
{
	return glm::max(glm::max(val.x, val.y), val.z);
}

glm::mat3 Math::GetCrossProductMatrix(const glm::vec3& vec)
{
	return glm::mat3
	{ 0.f, vec.z, -vec.y, -vec.z, 0.f, vec.x, vec.y, -vec.x, 0.f };
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

Mesh Math::GenerateBox(const glm::vec3& size)
{
	const glm::vec3 LeftFrontDown = { -size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
	const glm::vec3 RightFrontDown = { size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
	const glm::vec3 RightFrontUp = { size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };
	const glm::vec3 LeftFrontUp = { -size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };

	const glm::vec3 LeftBackDown = { -size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
	const glm::vec3 RightBackDown = { size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
	const glm::vec3 RightBackUp = { size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };
	const glm::vec3 LeftBackUp = { -size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };

	std::vector<Vertex> vertices;
	vertices.resize(24);

	//front
	vertices[0].Position = LeftFrontDown;
	vertices[1].Position = RightFrontDown;
	vertices[2].Position = RightFrontUp;
	vertices[3].Position = LeftFrontUp;

	//back
	vertices[4].Position = LeftBackDown;
	vertices[5].Position = RightBackDown;
	vertices[6].Position = RightBackUp;
	vertices[7].Position = LeftBackUp;

	//left
	vertices[8].Position = LeftFrontDown;
	vertices[9].Position = LeftBackDown;
	vertices[10].Position = LeftBackUp;
	vertices[11].Position = LeftFrontUp;

	//right
	vertices[12].Position = RightFrontDown;
	vertices[13].Position = RightBackDown;
	vertices[14].Position = RightBackUp;
	vertices[15].Position = RightFrontUp;

	//up
	vertices[16].Position = LeftFrontUp;
	vertices[17].Position = RightFrontUp;
	vertices[18].Position = RightBackUp;
	vertices[19].Position = LeftBackUp;

	//down
	vertices[20].Position = LeftFrontDown;
	vertices[21].Position = RightFrontDown;
	vertices[22].Position = RightBackDown;
	vertices[23].Position = LeftBackDown;

	//front
	constexpr glm::vec3 front = { 0.f, 0.f, 1.f };
	vertices[0].Normal = front;
	vertices[1].Normal = front;
	vertices[2].Normal = front;
	vertices[3].Normal = front;

	//back
	constexpr glm::vec3 back = { 0.f, 0.f, -1.f };
	vertices[4].Normal = back;
	vertices[5].Normal = back;
	vertices[6].Normal = back;
	vertices[7].Normal = back;

	//left
	constexpr glm::vec3 left = { -1.f, 0.f, 0.f };
	vertices[8].Normal = left;
	vertices[9].Normal = left;
	vertices[10].Normal = left;
	vertices[11].Normal = left;

	//right
	constexpr glm::vec3 right = { 1.f, 0.f, 0.f };
	vertices[12].Normal = right;
	vertices[13].Normal = right;
	vertices[14].Normal = right;
	vertices[15].Normal = right;

	//up
	constexpr glm::vec3 up = { 0.f, 1.f, 0.f };
	vertices[16].Normal = up;
	vertices[17].Normal = up;
	vertices[18].Normal = up;
	vertices[19].Normal = up;

	//down
	constexpr glm::vec3 down = { 0.f, -1.f, 0.f };
	vertices[20].Normal = down;
	vertices[21].Normal = down;
	vertices[22].Normal = down;
	vertices[23].Normal = down;

	std::vector<std::array<int, 3>> faces;
	faces.resize(12);

	//front
	faces[0] = { 0, 1, 2 };
	faces[1] = { 2, 3, 0 };

	//back
	faces[2] = { 6,5,4 };
	faces[3] = { 7,6,4 };

	//left
	faces[4] = { 10, 9, 8 };
	faces[5] = { 10, 8, 11 };

	//right
	faces[6] = { 12,13,14 };
	faces[7] = { 12,14,15 };

	//up
	faces[8] = { 16,17,18 };
	faces[9] = { 16,18,19 };

	//down
	faces[10] = { 22,21,20 };
	faces[11] = { 23,22,20 };

	Mesh box;

	box.vertices = std::move(vertices);
	box.indices.reserve(faces.size() * 3);
	for (auto& [i1, i2, i3] : faces)
	{
		box.indices.push_back(i1);
		box.indices.push_back(i2);
		box.indices.push_back(i3);
	}

	return box;
}

std::tuple<glm::mat3, float> Math::ComputeInertiaTensor(const Mesh& mesh, float expectedTotalWeight)
{
	const float pointMass = expectedTotalWeight / static_cast<float>(mesh.vertices.size());

	glm::vec3 centerOfMas = glm::vec3{ 0.f };
	for(auto& v:mesh.vertices)
	{
		centerOfMas += pointMass * v.Position;
	}
	centerOfMas /= expectedTotalWeight;

	glm::mat3 InertiaTensor{ 0.f };

	for (auto& v:mesh.vertices)
	{
		auto r = v.Position - centerOfMas;
		glm::mat3 rMatrix = Math::GetCrossProductMatrix(r);
		InertiaTensor += pointMass * rMatrix * rMatrix;
	}
	InertiaTensor *= -1.f;

	std::cout << glm::to_string(centerOfMas) << std::endl;
	std::cout << glm::to_string(InertiaTensor) << std::endl;

	return { InertiaTensor, expectedTotalWeight};
}
