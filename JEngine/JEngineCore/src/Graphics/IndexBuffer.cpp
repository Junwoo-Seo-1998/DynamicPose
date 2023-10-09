#include "IndexBuffer.h"

#include <cassert>
#include <glad/glad.h>


IndexBuffer::~IndexBuffer()
{
}

std::shared_ptr<IndexBuffer> IndexBuffer::CreateIndexBuffer(int size)
{
	return std::shared_ptr<IndexBuffer>(new IndexBuffer{ size });
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
}

void IndexBuffer::BindToVertexArray() const
{
	assert(m_Size >= 0, "didn't set size");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
}

void IndexBuffer::UnBind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetData(int size, const unsigned int* data, unsigned offset)
{
	m_Size = size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, sizeof(const unsigned int) * size, data);
}

int IndexBuffer::GetSize() const
{
	return m_Size;
}

IndexBuffer::IndexBuffer(int size)
	:m_Buffer(0), m_Size(size)
{
	CreateBuffer(sizeof(unsigned int) * size, nullptr);
}

void IndexBuffer::CreateBuffer(unsigned size, const void* data)
{
	glGenBuffers(1, &m_Buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
