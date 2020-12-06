#include "Material.h"

#include "GraphicsEngine.h"

#include <stdexcept>


Material::Material(const wchar_t* vertex_shader_path, const wchar_t* pixel_shader_path)
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(vertex_shader_path, "vsmain", &shader_byte_code, &size_shader);
	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	if (!m_vertex_shader) throw std::runtime_error("Material not created successfully.");

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(pixel_shader_path, "psmain", &shader_byte_code, &size_shader);
	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	if (!m_pixel_shader) throw std::runtime_error("Material not created successfully.");
}

Material::~Material()
{
}

void Material::addTexture(const TexturePtr& texture)
{
	m_vec_textures.push_back(texture);
}

void Material::removeTexture(unsigned int index)
{
	if (index >= m_vec_textures.size()) return;

	m_vec_textures.erase(m_vec_textures.begin() + index);
}
