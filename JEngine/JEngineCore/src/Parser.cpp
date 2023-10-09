#include "Parser.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <filesystem>

#include "AssimpGLMHelpers.h"


Model AssimpParser::ParseModel(const std::string& file_name)
{
	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_FlipUVs;
	const aiScene* scene = importer.ReadFile(file_name, flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return{};
	}
	Model model;
	model.name = std::filesystem::path(file_name).stem().string();
	model.root = ProcessNode(scene, scene->mRootNode, model.boneInfoMap, model.boneCounter);
	return model;
}


ModelNode AssimpParser::ProcessNode(const aiScene* scene, aiNode* node, std::map<std::string, BoneInfo>& _BoneInfoMap, int& _BoneCounter)
{
	ModelNode model;
	model.name = node->mName.data;
	model.nodeToParent = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model.meshes.emplace_back(ProcessMesh(scene, mesh, _BoneInfoMap, _BoneCounter));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		model.children.emplace_back(ProcessNode(scene, node->mChildren[i], _BoneInfoMap, _BoneCounter));
	}

	return model;
}

Mesh AssimpParser::ProcessMesh(const aiScene* scene, aiMesh* _mesh, std::map<std::string, BoneInfo>& _BoneInfoMap, int& _BoneCounter)
{
	Mesh mesh;
	mesh.name = _mesh->mName.data;
	mesh.vertices.resize(_mesh->mNumVertices);

	for (unsigned int i = 0; i < _mesh->mNumVertices; ++i)
	{
		aiVector3D& vertex = _mesh->mVertices[i];
		mesh.vertices[i].Position = { vertex.x, vertex.y, vertex.z };

		//SetVertexBoneDataToDefault
		for (int b = 0; b < mesh.vertices[i].BoneIDs.size(); b++)
		{
			mesh.vertices[i].BoneIDs[b] = -1;
			mesh.vertices[i].Weights[b] = 0.0f;
		}

		if (_mesh->HasNormals())
		{
			aiVector3D& normal = _mesh->mNormals[i];
			mesh.vertices[i].Normal = { normal.x, normal.y, normal.z };
		}
	}

	for (unsigned int f = 0; f < _mesh->mNumFaces; ++f)
	{
		mesh.indices.emplace_back(_mesh->mFaces[f].mIndices[0]);
		mesh.indices.emplace_back(_mesh->mFaces[f].mIndices[1]);
		mesh.indices.emplace_back(_mesh->mFaces[f].mIndices[2]);
	}

	//ExtractBoneWeightForVertices
	{
		auto& boneInfoMap = _BoneInfoMap;

		//for bone id
		int& boneCount = _BoneCounter;

		for (int boneIndex = 0; boneIndex < _mesh->mNumBones; ++boneIndex)
		{
			//to be used later
			int boneID = -1;

			std::string boneName = _mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				//new
				BoneInfo newBoneInfo;

				newBoneInfo.id = boneCount;
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_mesh->mBones[boneIndex]->mOffsetMatrix);

				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				//already exist so use it
				boneID = boneInfoMap[boneName].id;
			}

			assert(boneID != -1);

			auto weights = _mesh->mBones[boneIndex]->mWeights;
			int numWeights = _mesh->mBones[boneIndex]->mNumWeights;

			//set weights for each verts
			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId; //index of vert that bone is affecting
				float weight = weights[weightIndex].mWeight;//weight of bone

				assert(vertexId <= mesh.vertices.size());//should be in range

				//SetVertexBoneData
				for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
				{
					if (mesh.vertices[vertexId].BoneIDs[i] < 0)//if not assigned yet
					{
						//assign
						mesh.vertices[vertexId].Weights[i] = weight; 
						mesh.vertices[vertexId].BoneIDs[i] = boneID;
						break;
					}
				}
			}
		}
	}
	return mesh;
}



Animation AssimpParser::ParseAnimation(const std::string& file_name, Model& model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_name, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	Animation ani;
	for (int i=0; i<scene->mNumAnimations; ++i)
	{
		std::cout << scene->mAnimations[i]->mName.C_Str() << std::endl;
	}

	aiAnimation* animation = scene->mAnimations[16];

	ani.name = animation->mName.C_Str();
	ani.duration = animation->mDuration;
	ani.ticksPerSecond = animation->mTicksPerSecond;

	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();

	ani.rootNode = ProcessAnimationNode(scene->mRootNode);

	int size = animation->mNumChannels;

	auto& boneInfoMap = model.boneInfoMap;//getting m_BoneInfoMap from Model class
	int& boneCount = model.boneCounter; //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		ani.bones.push_back(Bone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}

	ani.boneInfoMap = boneInfoMap;
	return ani;
}


AssimpNodeData AssimpParser::ProcessAnimationNode(const aiNode* src)
{
	assert(src);
	AssimpNodeData dest;

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData = ProcessAnimationNode(src->mChildren[i]);
		dest.children.push_back(newData);
	}
	return  dest;
}