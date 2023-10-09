#pragma once
#include <vector>
#include "Mesh.h"
#include <string>
#include <map>




struct ModelNode
{
    // model data
    std::string name;
    std::vector<ModelNode> children;
    std::vector<Mesh> meshes;
    glm::mat4 nodeToParent;
};

struct Model
{
    std::string name;
    ModelNode root;

    std::map<std::string, BoneInfo> boneInfoMap;
    int boneCounter = 0;
};


