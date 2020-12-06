// Copyright (c) 2019  PardCode.
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#pragma once

#include "Prerequisites.h"

#include "RenderSystem.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "Material.h"

#include <d3d11.h>


class GraphicsEngine
{

private:
	// Initialize the GraphicsEngine and DirectX 11 Device
	GraphicsEngine();
	// Release all the resources loaded
	~GraphicsEngine();

public:
	RenderSystem* getRenderSystem();
	TextureManager* getTextureManager();
	MeshManager* getMeshManager();

	MaterialPtr createMaterial(const wchar_t* vertex_shader_path, const wchar_t* pixel_shader_path);
	void setMaterial(const MaterialPtr& material);

	void getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size);
	static GraphicsEngine* get();
	static void create();
	static void release();

private:
	RenderSystem* m_render_system = nullptr;
	TextureManager* m_tex_manager = nullptr;
	MeshManager* m_mesh_manager = nullptr;
	static GraphicsEngine* m_engine;

	unsigned char m_mesh_layout_byte_code[1024];
	size_t m_mesh_layout_size = 0;

};
