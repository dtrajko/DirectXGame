#pragma once

#include <d3d11.h>

#include "Resource.h"


class Texture : public Resource
{
public:
	Texture();
	Texture(const wchar_t* full_path);
	~Texture();

private:
	ID3D11Resource* m_texture = nullptr;

};
