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

std::shared_ptr<Animation> AssimpParser::ParseAnimation(const std::string& file_name, int index)
{
	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_FlipUVs;
	const aiScene* scene = importer.ReadFile(file_name, flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return{};
	}

	aiAnimation* animation = scene->mAnimations[index];

	std::shared_ptr<Animation> toReturn = std::make_shared<Animation>();
	Animation& ani = *toReturn;

	ani.AnimationName = animation->mName.C_Str();
	ani.Duration = static_cast<float>(animation->mDuration);
	ani.TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);
	
	int size = static_cast<int>(animation->mNumChannels);
	ani.Channels.reserve(size);
	for (int i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		Channel channelStruct;
		channelStruct.Name = channel->mNodeName.C_Str();

		//parse data
		int NumPostions = channel->mNumPositionKeys;
		channelStruct.Positions.reserve(NumPostions);
		for (int positionIndex = 0; positionIndex < NumPostions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			channelStruct.Positions.push_back(data);
		}

		int NumRotations = channel->mNumRotationKeys;
		channelStruct.Rotations.reserve(NumRotations);
		for (int rotationIndex = 0; rotationIndex < NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
			KeyRotation data;
			data.Rotation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			channelStruct.Rotations.push_back(data);
		}

		int NumScalings = channel->mNumScalingKeys;
		channelStruct.Scales.reserve(NumScalings);
		for (int keyIndex = 0; keyIndex < NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(scale);
			data.timeStamp = timeStamp;
			channelStruct.Scales.push_back(data);
		}

		ani.Channels.push_back(channelStruct);
		ani.ChannelsMap[channelStruct.Name] = channelStruct;
	}


	return toReturn;
}

std::vector<std::shared_ptr<Animation>> AssimpParser::ParseAnimations(const std::string& file_name)
{
	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_FlipUVs;
	const aiScene* scene = importer.ReadFile(file_name, flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return{};
	}

	std::vector<std::shared_ptr<Animation>> toReturn;
	int Animations = scene->mNumAnimations;
	toReturn.reserve(Animations);
	for (int a=0; a<Animations; ++a)
	{
		aiAnimation* animation = scene->mAnimations[a];
		std::shared_ptr<Animation> toAdd = std::make_shared<Animation>();
		Animation& ani = *toAdd;

		ani.AnimationName = animation->mName.C_Str();
		ani.Duration = static_cast<float>(animation->mDuration);
		ani.TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);

		int size = static_cast<int>(animation->mNumChannels);
		ani.Channels.reserve(size);
		for (int i = 0; i < size; i++)
		{
			aiNodeAnim* channel = animation->mChannels[i];
			Channel channelStruct;
			channelStruct.Name = channel->mNodeName.C_Str();

			//parse data
			int NumPostions = channel->mNumPositionKeys;
			channelStruct.Positions.reserve(NumPostions);
			for (int positionIndex = 0; positionIndex < NumPostions; ++positionIndex)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				float timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
				KeyPosition data;
				data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
				data.timeStamp = timeStamp;
				channelStruct.Positions.push_back(data);
			}

			int NumRotations = channel->mNumRotationKeys;
			channelStruct.Rotations.reserve(NumRotations);
			for (int rotationIndex = 0; rotationIndex < NumRotations; ++rotationIndex)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				float timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
				KeyRotation data;
				data.Rotation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
				data.timeStamp = timeStamp;
				channelStruct.Rotations.push_back(data);
			}

			int NumScalings = channel->mNumScalingKeys;
			channelStruct.Scales.reserve(NumScalings);
			for (int keyIndex = 0; keyIndex < NumScalings; ++keyIndex)
			{
				aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
				float timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
				KeyScale data;
				data.scale = AssimpGLMHelpers::GetGLMVec(scale);
				data.timeStamp = timeStamp;
				channelStruct.Scales.push_back(data);
			}

			ani.Channels.push_back(channelStruct);
			ani.ChannelsMap[channelStruct.Name] = channelStruct;
		}
		toReturn.push_back(toAdd);
	}
	return toReturn;
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

		int numBones = static_cast<int>(_mesh->mNumBones);
		mesh.skinned = numBones > 0;

		for (int boneIndex = 0; boneIndex < numBones; ++boneIndex)
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

