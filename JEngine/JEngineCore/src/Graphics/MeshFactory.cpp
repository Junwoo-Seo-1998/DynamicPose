#include "MeshFactory.h"
#include "Mesh.h"

struct Face
{
	unsigned i1, i2, i3;
};

std::shared_ptr<Mesh> MeshFactory::CreateBox(const glm::vec3& size)
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

	std::vector<Face> faces;
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

	std::shared_ptr<Mesh> to_return = std::make_shared<Mesh>();


	to_return->vertices = vertices;

	to_return->indices.reserve(faces.size() * 3);
	for (auto& [i1, i2, i3] : faces)
	{
		to_return->indices.push_back(i1);
		to_return->indices.push_back(i2);
		to_return->indices.push_back(i3);
	}

	return to_return;
}
