#include "VertexShader.h"
#include "RenderSystem.h"

#include <exception>


VertexShader::VertexShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system): m_system(system)
{
	HRESULT hr = m_system->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs);
	if (FAILED(hr))
	{
		throw std::exception("PixelShader failed to initialize.");
	}
}

VertexShader::~VertexShader()
{
	m_vs->Release();
}
