#pragma once

#include "VertexShader.h"
#include "PixelShader.h"

#include <vector>


class Material
{
public:
	Material(const wchar_t* vertex_shader_path, const wchar_t* pixel_shader_path);
	~Material();

	void addTexture(const TexturePtr& texture);
	void removeTexture(unsigned int index);

private:
	VertexShaderPtr m_vertex_shader;
	PixelShaderPtr m_pixel_shader;
	std::vector<TexturePtr> m_vec_textures;

};
