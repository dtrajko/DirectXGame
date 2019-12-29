#include "AppWindow.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"

#include <iostream>
#include <Windows.h>



struct vertex
{
	Vector3D position;
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

	InputSystem::get()->addListener(this);

	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rect = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rect.right - rect.left, rect.bottom - rect.top);

	vertex vertex_list[] =
	{
		//          POSITION                      COLOR                       COLOR
		//          X   -  Y  -  Z                R  -  G  -  B               R  -  G  -  B
		// Front face
		{ Vector3D(-0.5f, -0.5f, -0.5f), Vector3D(1.0f, 0.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f) }, // POS0
		{ Vector3D(-0.5f,  0.5f, -0.5f), Vector3D(0.0f, 1.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f) }, // POS1
		{ Vector3D( 0.5f,  0.5f, -0.5f), Vector3D(0.0f, 0.0f, 1.0f), Vector3D(0.0f, 0.0f, 1.0f) }, // POS2
		{ Vector3D( 0.5f, -0.5f, -0.5f), Vector3D(1.0f, 1.0f, 0.0f), Vector3D(1.0f, 1.0f, 0.0f) }, // POS3

		// Back face
		{ Vector3D( 0.5f, -0.5f,  0.5f), Vector3D(1.0f, 1.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f) }, // POS4
		{ Vector3D( 0.5f,  0.5f,  0.5f), Vector3D(0.0f, 0.0f, 1.0f), Vector3D(0.0f, 1.0f, 0.0f) }, // POS5
		{ Vector3D(-0.5f,  0.5f,  0.5f), Vector3D(0.0f, 1.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f) }, // POS6
		{ Vector3D(-0.5f, -0.5f,  0.5f), Vector3D(1.0f, 0.0f, 0.0f), Vector3D(1.0f, 1.0f, 0.0f) }, // POS7
	};

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_vertex_list = ARRAYSIZE(vertex_list);

	unsigned int index_list[] =
	{
		// Front side
		0, 1, 2, // First triangle
		2, 3, 0, // Second triangle
		// Back side
		4, 5, 6,
		6, 7, 4,
		// Top side
		1, 6, 5,
		5, 2, 1,
		// Bottom side
		7, 0, 3,
		3, 4, 7,
		// Right side
		3, 2, 5,
		5, 4, 3,
		// Left side
		7, 6, 1,
		1, 0, 7,
	};

	m_ib = GraphicsEngine::get()->createIndexBuffer();
	UINT size_index_list = ARRAYSIZE(index_list);

	m_ib->load(index_list, size_index_list);


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	// vertex shader
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(vertex_list, sizeof(vertex), size_vertex_list, shader_byte_code, size_shader);
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

	InputSystem::get()->update();

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

	// Set index buffer
	GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);

	// Finally draw the triangle
	// GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64(); // time in milliseconds
	m_delta_time = m_old_delta ? ((m_new_delta - m_old_delta) / 1000.0f) : 0.0f;
}

void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = (unsigned int)::GetTickCount64();

	cc.m_world.setIdentity();
	Matrix4x4 temp;
	temp.setIdentity();

	// Scaling
	// temp.setIdentity();
	// m_delta_scale += m_delta_time / 1.0f;
	// float delta_scale_sin = (sin(m_delta_scale) + 1.0f) / 2.0f;
	// Vector3D start_scale = Vector3D(0.6f, 0.6f, 0.0f);
	// Vector3D end_scale = Vector3D(1.2f, 1.2f, 0.0f);
	// temp.setScale(Vector3D::lerp(start_scale, end_scale, delta_scale_sin));
	// cc.m_world *= temp;

	cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));

	// Rotation
	m_delta_rot += m_delta_time / 1.0f;

	temp.setIdentity();
	temp.setRotationZ(0.0f);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	cc.m_world *= temp;

	// Translation
	// temp.setIdentity();
	// m_delta_pos += (m_delta_time / 2.0f);
	// float delta_pos_cos = sin(m_delta_pos);
	// // std::cout << "delta_pos_sin: " << delta_pos_cos << std::endl;
	// 
	// Vector3D start_pos = Vector3D(0.0f, 0.0f, 0.0f);
	// Vector3D end_pos = Vector3D(1.6f, 0.0f, 0.0f);
	// temp.setTranslation(Vector3D::lerp(start_pos, end_pos, delta_pos_cos));
	// // cc.m_world.setTranslation(Vector3D(0.0f, 0.0f, 0.0f))
	// cc.m_world *= temp;

	cc.m_view.setIdentity();

	RECT window_rect = this->getClientWindowRect();
	cc.m_proj.setOrthoLH(
		(window_rect.right - window_rect.left) / 200.0f,
		(window_rect.bottom - window_rect.top) / 200.0f,
		-4.0f,
		4.0f
	);

	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_ib->release();
	m_cb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	switch (key)
	{
	case 'W':
		m_rot_x += 3.0f * m_delta_time;
		break;
	case 'S':
		m_rot_x -= 3.0f * m_delta_time;
		break;
	case 'A':
		m_rot_y += 3.0f * m_delta_time;
		break;
	case 'D':
		m_rot_y -= 3.0f * m_delta_time;
		break;
	}
}

void AppWindow::onKeyUp(int key)
{
}

void AppWindow::onMouseMove(const Point& delta_mouse_pos)
{
	m_rot_x -= delta_mouse_pos.m_y * m_delta_time * 0.4f;
	m_rot_y -= delta_mouse_pos.m_x * m_delta_time * 0.4f;
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 0.5f;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 1.5f;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onMiddleMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 2.0f;
}

void AppWindow::onMiddleMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

AppWindow::~AppWindow()
{

}
