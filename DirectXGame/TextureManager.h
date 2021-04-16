#pragma once

#include "ResourceManager.h"
#include "Texture.h"


class TextureManager : public ResourceManager
{
public:
	TextureManager();
	virtual ~TextureManager() override;

	TexturePtr createTextureFromFile(const wchar_t* file_path);
	TexturePtr createTexture(const Rect& size, Texture::Type type);

protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path) override;

};
