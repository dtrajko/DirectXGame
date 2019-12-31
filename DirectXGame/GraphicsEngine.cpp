#include "GraphicsEngine.h"
#include "RenderSystem.h"

#include <d3dcompiler.h>
#include <exception>


GraphicsEngine::GraphicsEngine()
{
	init();
}

bool GraphicsEngine::init()
{
	m_render_system = new RenderSystem();
	m_render_system->init();
	return true;
}

GraphicsEngine* GraphicsEngine::get()
{
	static GraphicsEngine engine;
	return &engine;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

GraphicsEngine::~GraphicsEngine()
{
	delete m_render_system;
}
