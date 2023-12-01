#pragma once
#include <memory>
#include <string>
#include <vector>

#include "GraphicData.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"

class Mesh {
public:
    // mesh data
    std::string name;
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    bool skinned = false;
};


class MeshInstance
{
public:
    MeshInstance() = default;
    MeshInstance(Mesh& mesh);

    std::shared_ptr<VertexBuffer> m_Buffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};