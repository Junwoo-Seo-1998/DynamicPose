#pragma once
#include <memory>

class VertexArray
{
public:
	virtual ~VertexArray();
	static std::shared_ptr<VertexArray> CreateVertexArray();
	//void AttachBuffer(const VertexBuffer& buffer, bool ManualVAO = false);
	//void AttachBuffer(const ElementBuffer& buffer, bool ManualVAO = false);

	void Bind() const;
	void UnBind() const;
private:
	VertexArray();
private:
	unsigned m_VertexArray;
};