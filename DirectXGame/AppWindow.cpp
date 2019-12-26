#include "AppWindow.h"


struct vec3
{
	float x;
	float y;
	float z;
};

struct vertex
{
	vec3 position;
};

AppWindow::AppWindow()
{

}

void AppWindow::onCreate()
{
	Window::onCreate();
	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rect = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rect.right - rect.left, rect.bottom - rect.top);

	vertex list[] =
	{
		// X - Y - Z
		{ -0.5f, -0.5f, 0.0f }, // POS1
		{ -0.5f,  0.5f, 0.0f},  // POS2
		{  0.5f, -0.5f, 0.0f},  // POS3
		{  0.5f,  0.5f, 0.0f},  // POS4
	};

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	GraphicsEngine::get()->createShaders();

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	// Clear the render target
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0.2f, 0.0f, 0.8f, 1);

	// Set Viewport of render target in which we have to draw
	RECT rect = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rect.right - rect.left, rect.bottom - rect.top);

	// Set default shader in the graphics pipeline to be able to draw 
	GraphicsEngine::get()->setShaders();
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);

	// Set the vertices of the triangle to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	// Finally draw the triangle
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);

	m_swap_chain->present(false);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	GraphicsEngine::get()->release();
}

AppWindow::~AppWindow()
{

}
