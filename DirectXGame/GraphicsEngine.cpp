#include "GraphicsEngine.h"
#include "RenderSystem.h"

#include <d3dcompiler.h>
#include <exception>


GraphicsEngine::GraphicsEngine()
{
	m_render_system = new RenderSystem();
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
	m_render_system->release();
	delete m_render_system;
}
