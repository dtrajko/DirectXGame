#pragma once

#include <d3d11.h>

#include "Resource.h"


class Texture : public Resource
{
public:
	Texture();
	Texture(const wchar_t* full_path);
	virtual ~Texture() override;

private:
	ID3D11Resource* m_texture = nullptr;
	ID3D11ShaderResourceView* m_shader_res_view = nullptr;
	ID3D11SamplerState* m_sampler_state = nullptr;

private:
	friend class DeviceContext;

};
