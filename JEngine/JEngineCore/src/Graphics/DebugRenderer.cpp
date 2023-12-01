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
static std::shared_ptr<Shader> DebugShader;
static std::vector<glm::vec3> linePointsToDraw;
static std::shared_ptr<MeshInstance> sphereInstance;
void DebugRenderer::Init()
{
	vao = VertexArray::CreateVertexArray();
	vbo = VertexBuffer::CreateVertexBuffer(sizeof(glm::vec3) * 2 * 10000);
	vbo->SetDataTypes({
		{0, DataType::Float3},
		});
	linePointsToDraw.reserve(10000 * 2);

	ShaderSource source{};
	source[ShaderType::VertexShader] = { "Shader/line.vert" };
	source[ShaderType::FragmentShader] = { "Shader/line.frag" };
	DebugShader = Shader::CreateShaderFromFile(source);

	sphereInstance = std::make_shared<MeshInstance>(Math::GenerateSpherePointsWithIndices());
}

void DebugRenderer::Shutdown()
{
}

void DebugRenderer::SetViewProjection(const glm::mat4& viewProjection)
{
	vao->Bind();
	DebugShader->Use();
	DebugShader->SetMat4("ViewProjection", viewProjection);
}

void DebugRenderer::BeginDrawLine(const glm::mat4& viewProjection, const glm::vec3& color)
{
	SetViewProjection(viewProjection);
	DebugShader->SetMat4("Model", glm::identity<glm::mat4>());
	DebugShader->SetFloat3("Color", color);
}

void DebugRenderer::EndDrawLine()
{
	vbo->BindToVertexArray();
	vbo->SetData(static_cast<int>(linePointsToDraw.size()) * sizeof(glm::vec3), linePointsToDraw.data());
	glDrawArrays(GL_LINES, 0, static_cast<int>(linePointsToDraw.size()));
	linePointsToDraw.clear();
}

void DebugRenderer::DrawLine(const glm::vec3& p1, const glm::vec3& p2)
{
	linePointsToDraw.push_back(p1);
	linePointsToDraw.push_back(p2);
}


void DebugRenderer::DrawSphere(const glm::vec3& pos, float rad, const glm::vec3& color)
{
	DebugShader->SetMat4("Model", glm::translate(glm::mat4(1.0f), pos)
		* glm::toMat4(glm::quat{ 1.f, {0.f,0.f,0.f} })
		* glm::scale(glm::mat4(1.0f), glm::vec3(rad)));
	DebugShader->SetFloat3("Color", color);
	sphereInstance->m_Buffer->BindToVertexArray();
	sphereInstance->m_IndexBuffer->BindToVertexArray();
	glDrawElements(GL_TRIANGLES, sphereInstance->m_IndexBuffer->GetSize(), GL_UNSIGNED_INT, nullptr);
}
