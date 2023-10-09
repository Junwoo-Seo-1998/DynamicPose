#pragma once
#include <glm/fwd.hpp>

class DebugRenderer
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const glm::mat4& viewProjection, const glm::vec3& color);
	static void EndScene();

	static void DrawLine(const glm::vec3& p1, const glm::vec3& p2);
};
