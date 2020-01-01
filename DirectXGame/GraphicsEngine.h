#pragma once

#include <d3d11.h>

#include "Prerequisites.h"


class GraphicsEngine
{

public:
	GraphicsEngine();
	RenderSystem* getRenderSystem();
	static GraphicsEngine* get();
	~GraphicsEngine();

private:
	RenderSystem* m_render_system = nullptr;

};
