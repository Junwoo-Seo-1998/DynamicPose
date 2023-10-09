#pragma once
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <glm/vec3.hpp>

#include "Animation.h"
#include "Mesh.h"
#include "Model.h"

struct aiScene;
struct aiNode;

class AssimpParser
{
public:
	static Model ParseModel(const std::string& file_name);

	static Animation ParseAnimation(const std::string& file_name, Model& model);

private:
	static ModelNode ProcessNode(const aiScene* scene, aiNode* node, std::map<std::string, BoneInfo>& _BoneInfoMap, int& _BoneCounter);

	static Mesh ProcessMesh(const aiScene* scene, aiMesh* mesh, std::map<std::string, BoneInfo>& _BoneInfoMap, int& _BoneCounter);

	static AssimpNodeData ProcessAnimationNode(const aiNode* src);
};
