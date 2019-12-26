#include "VertexShader.h"
#include "GraphicsEngine.h"


VertexShader::VertexShader()
{

}

bool VertexShader::init(const void* shader_byte_code, size_t byte_code_size)
{
	HRESULT hr = GraphicsEngine::get()->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void VertexShader::release()
{
	m_vs->Release();
	delete this;
}

VertexShader::~VertexShader()
{
	
}
