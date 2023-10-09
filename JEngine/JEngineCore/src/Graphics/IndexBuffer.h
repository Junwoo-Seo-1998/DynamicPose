#pragma once
#include <memory>
#include <vector>
class IndexBuffer
{
public:
	IndexBuffer() = delete;
	virtual ~IndexBuffer();
	static std::shared_ptr<IndexBuffer> CreateIndexBuffer(int size);
	void Bind() const;
	void BindToVertexArray() const;
	void UnBind() const;
	void SetData(int size, const unsigned int* data, unsigned offset = 0);
	int GetSize() const;
private:
	IndexBuffer(int size);
	void CreateBuffer(unsigned size, const void* data);

	unsigned m_Buffer;
	int m_Size = -1;
};
