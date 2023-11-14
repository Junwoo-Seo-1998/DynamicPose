#include "DebugRenderer.h"
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Components.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

static std::shared_ptr<VertexArray> vao;
static std::shared_ptr<VertexBuffer> vbo;
static std::shared_ptr<Shader> lineShader;
static std::vector<glm::vec3> toDraw;
void DebugRenderer::Init()
{
	vao = VertexArray::CreateVertexArray();
	vbo = VertexBuffer::CreateVertexBuffer(sizeof(glm::vec3) * 2 * 10000);
	vbo->SetDataTypes({
		{0, DataType::Float3},
		});
	toDraw.reserve(10000 * 2);

	ShaderSource source{};
	source[ShaderType::VertexShader] = { "Shader/line.vert" };
	source[ShaderType::FragmentShader] = { "Shader/line.frag" };
	lineShader = Shader::CreateShaderFromFile(source);
}

void DebugRenderer::Shutdown()
{
}

void DebugRenderer::BeginScene(const glm::mat4& viewProjection, const glm::vec3& color)
{
	vao->Bind();
	lineShader->Use();
	lineShader->SetMat4("Matrix", viewProjection);
	lineShader->SetFloat3("Color", color);
}

void DebugRenderer::EndScene()
{
	vbo->BindToVertexArray();
	vbo->SetData(static_cast<int>(toDraw.size()) * sizeof(glm::vec3), toDraw.data());
	glDrawArrays(GL_LINES, 0, static_cast<int>(toDraw.size()));
	toDraw.clear();
}

void DebugRenderer::DrawLine(const glm::vec3& p1, const glm::vec3& p2)
{
	toDraw.push_back(p1);
	toDraw.push_back(p2);
}

