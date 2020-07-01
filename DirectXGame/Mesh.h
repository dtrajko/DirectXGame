#pragma once

#include "Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


class Mesh : public Resource
{
public:
	Mesh();
	Mesh(const wchar_t* full_path);
	virtual ~Mesh() override;

private:
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;

};
