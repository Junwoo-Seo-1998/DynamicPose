#pragma once
#include <memory>
#include <glm/vec3.hpp>

class Mesh;

class MeshFactory
{
public:
	static std::shared_ptr<Mesh> CreateBox(const glm::vec3& size);
};
