#pragma once
#include <glm/fwd.hpp>

class DebugRenderer
{
public:
	static void Init();
	static void Shutdown();

	static void SetViewProjection(const glm::mat4& viewProjection);
	static void BeginDrawLine(const glm::mat4& viewProjection, const glm::vec3& color);
	static void EndDrawLine();

	static void DrawLine(const glm::vec3& p1, const glm::vec3& p2);
	static void DrawSphere(const glm::vec3& pos, float rad, const glm::vec3& color);
};
