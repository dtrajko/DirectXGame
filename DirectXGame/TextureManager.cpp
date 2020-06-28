#include "TextureManager.h"

#include "Texture.h"


TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

Resource* TextureManager::createResourceFromFileConcrete(const wchar_t* file_path)
{
	Texture* tex = nullptr;
	try
	{
		tex = new Texture(file_path);
	}
	catch (...) {};

	return tex;
}
