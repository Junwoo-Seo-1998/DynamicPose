#pragma once
#include <array>
#include <glm/glm.hpp>
#define MAX_BONE_INFLUENCE 4

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;
};

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    //glm::vec2 TexCoords;

    //bone indexes which will influence this vertex
    std::array<int, MAX_BONE_INFLUENCE>BoneIDs;
	//int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    std::array<float, MAX_BONE_INFLUENCE>Weights;
    //float m_Weights[MAX_BONE_INFLUENCE];
};


