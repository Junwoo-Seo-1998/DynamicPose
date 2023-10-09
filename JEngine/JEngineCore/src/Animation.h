#pragma once
#include <map>
#include <string>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>

#include "Bone.h"
#include "GraphicData.h"


struct Model;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};
struct Animation
{
	std::string name;
	float duration;
	int ticksPerSecond;
	AssimpNodeData rootNode;
	std::vector<Bone> bones;
	std::map<std::string, BoneInfo> boneInfoMap;

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(bones.begin(), bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == bones.end()) return nullptr;
		else return &(*iter);
	}

};

