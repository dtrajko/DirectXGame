#include "AppWindow.h"
#include "Vector3D.h"
#include "Matrix4x4.h"

#include <iostream>
#include <Windows.h>



struct vertex
{
	Vector3D position;
	Vector3D position1;
	Vector3D color;
	Vector3D color1;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
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
		//          POSITION                      POSITION 1                     COLOR                       COLOR
		//          X   -  Y  -  Z                X   -   Y  -   Z               R  -  G  -  B               R  -  G  -  B
		{ Vector3D(-0.5f, -0.5f, 0.0f), Vector3D(-0.32f, -0.11f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f) }, // POS1 RED
		{ Vector3D(-0.5f,  0.5f, 0.0f), Vector3D(-0.11f,  0.78f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f) }, // POS2 GREEN
		{ Vector3D( 0.5f, -0.5f, 0.0f), Vector3D( 0.75f, -0.73f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f), Vector3D(1.0f, 1.0f, 0.0f) }, // POS3 BLUE
		{ Vector3D( 0.5f,  0.5f, 0.0f), Vector3D( 0.88f,  0.77f, 0.0f), Vector3D(1.0f, 1.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f) }, // POS4 YELLOW
	};

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	// vertex shader
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();

	// pixel shader
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	// Clear the render target
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0.2f, 0.4f, 0.8f, 1);

	// Set Viewport of render target in which we have to draw
	RECT rect = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rect.right - rect.left, rect.bottom - rect.top);

	this->updateQuadPosition();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	// Set default shader in the graphics pipeline to be able to draw 
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	// Set the vertices of the triangle to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	// Finally draw the triangle
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64(); // time in milliseconds
	m_delta_time = m_old_delta ? ((m_new_delta - m_old_delta) / 1000.0f) : 0.0f;
}

void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = (unsigned int)::GetTickCount64();

	cc.m_world.setTranslation(Vector3D(0.0f, 0.0f, 0.0f));

	m_delta_scale += m_delta_time / 0.2f;
	float delta_scale_sin = (sin(m_delta_scale) + 1.0f) / 2.0f;
	Vector3D start_scale = Vector3D(0.6f, 0.6f, 0.0f);
	Vector3D end_scale = Vector3D(1.2f, 1.2f, 0.0f);
	cc.m_world.setScale(Vector3D::lerp(start_scale, end_scale, delta_scale_sin));

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
	{
		m_delta_pos = 0.0f;
	}
	Vector3D start_pos = Vector3D(-1.0f, -1.0f, 0.0f);
	Vector3D end_pos = Vector3D(1.0f, 1.0f, 0.0f);
	// cc.m_world.setTranslation(Vector3D::lerp(start_pos, end_pos, m_delta_pos));

	Matrix4x4 temp;
	temp.setTranslation(Vector3D::lerp(start_pos, end_pos, m_delta_pos));
	cc.m_world *= temp;

	cc.m_view.setIdentity();

	RECT window_rect = this->getClientWindowRect();
	cc.m_proj.setOrthoLH(
		(window_rect.right - window_rect.left) / 400.0f,
		(window_rect.bottom - window_rect.top) / 400.0f,
		-4.0f,
		4.0f
	);

	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);

}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}

AppWindow::~AppWindow()
{

}
