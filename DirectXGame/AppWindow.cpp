#include "AppWindow.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "GraphicsEngine.h"

#include <iostream>
#include <Windows.h>
#include <exception>


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
	m_render_system = GraphicsEngine::get()->getRenderSystem();
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	RECT rect = this->getClientWindowRect();
	m_swap_chain = m_render_system->createSwapChain(this->m_hwnd, rect.right - rect.left, rect.bottom - rect.top);

	m_world_cam.setTranslation(Vector3D(0.0f, 0.0f, -4.0f));

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

	// Vertex buffer
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	UINT size_vertex_list = ARRAYSIZE(vertex_list);

	// Index buffer
	UINT size_index_list = ARRAYSIZE(index_list);
	m_ib = m_render_system->createIndexBuffer(index_list, size_index_list);

	// vertex shader
	m_render_system->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = m_render_system->createVertexShader(shader_byte_code, size_shader);
	m_vb = m_render_system->createVertexBuffer(vertex_list, sizeof(vertex), size_vertex_list, shader_byte_code, size_shader);
	m_render_system->releaseCompiledShader();

	// pixel shader
	m_render_system->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = m_render_system->createPixelShader(shader_byte_code, size_shader);
	m_render_system->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	m_cb = m_render_system->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	width = (float)(this->getClientWindowRect().right - this->getClientWindowRect().left);
	height = (float)(this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	// Clear the render target
	m_render_system->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0.2f, 0.4f, 0.8f, 1);

	// Set Viewport of render target in which we have to draw
	RECT rect = this->getClientWindowRect();
	m_render_system->getImmediateDeviceContext()->setViewportSize(rect.right - rect.left, rect.bottom - rect.top);

	this->update();

	m_render_system->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	m_render_system->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	// Set default shader in the graphics pipeline to be able to draw 
	m_render_system->getImmediateDeviceContext()->setVertexShader(m_vs);
	m_render_system->getImmediateDeviceContext()->setPixelShader(m_ps);

	// Set the vertices of the triangle to draw
	m_render_system->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	// Set index buffer
	m_render_system->getImmediateDeviceContext()->setIndexBuffer(m_ib);

	// Finally draw the triangle
	// GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	m_render_system->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64(); // time in milliseconds
	m_delta_time = m_old_delta ? ((m_new_delta - m_old_delta) / 1000.0f) : 0.0f;
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = (unsigned int)::GetTickCount64();

	// // Scale
	// cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));
	// 
	// // Rotation
	// m_delta_rot += m_delta_time / 1.0f;
	// temp.setIdentity();
	// temp.setRotation(m_rot_x, m_rot_y, 0.0f);
	// cc.m_world *= temp;
	// 
	// // Translation
	// temp.setIdentity();
	// m_delta_pos += (m_delta_time / 2.0f);
	// float delta_pos_cos = sin(m_delta_pos);
	// 
	// Vector3D start_pos = Vector3D(0.0f, 0.0f, 0.0f);
	// Vector3D end_pos = Vector3D(1.6f, 0.0f, 0.0f);
	// temp.setTranslation(Vector3D::lerp(start_pos, end_pos, delta_pos_cos));
	// cc.m_world *= temp;

	cc.m_world.setIdentity();

	Matrix4x4 temp;
	Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;

	Vector3D new_pos =
		m_world_cam.getTranslation() +
		world_cam.getXDirection() * (m_right * cam_speed) +
		world_cam.getYDirection() * (m_up * cam_speed) +
		world_cam.getZDirection() * (m_forward * cam_speed);
	world_cam.setTranslation(new_pos);
	m_world_cam = world_cam;
	world_cam.inverse();

	cc.m_view = world_cam;

	float fov = 1.0f; // 1.57f;
	float aspect = width / height;
	float znear = 0.1f;
	float zfar = 100.0f;
	cc.m_proj.setPerspectiveFovLH(fov, aspect, znear, zfar);

	// cc.m_proj.setOrthoLH(
	// 	(window_rect.right - window_rect.left) / 200.0f,
	// 	(window_rect.bottom - window_rect.top) / 200.0f,
	// 	-4.0f,
	// 	4.0f
	// );

	m_cb->update(m_render_system->getImmediateDeviceContext(), &cc);
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
		// m_rot_x += 3.0f * m_delta_time;
		m_forward = 1.0f;
		break;
	case 'S':
		// m_rot_x -= 3.0f * m_delta_time;
		m_forward = -1.0f;
		break;
	case 'A':
		// m_rot_y += 3.0f * m_delta_time;
		m_right = -1.0f;
		break;
	case 'D':
		// m_rot_y -= 3.0f * m_delta_time;
		m_right = 1.0f;
		break;
	case 'Q':
		// m_rot_y += 3.0f * m_delta_time;
		m_up = -1.0f;
		break;
	case 'E':
		// m_rot_y -= 3.0f * m_delta_time;
		m_up =  1.0f;
		break;
	}
}

void AppWindow::onKeyUp(int key)
{
	m_right   = 0.0f;
	m_up      = 0.0f;
	m_forward = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	m_rot_x += (mouse_pos.m_y - (height / 2.0f)) * m_delta_time * mouse_speed;
	m_rot_y += (mouse_pos.m_x - (width / 2.0f)) * m_delta_time * mouse_speed;

	InputSystem::get()->setCursorPosition(Point((int)(width / 2.0f), (int)(height / 2.0f)));
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

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

AppWindow::~AppWindow()
{
	delete m_ps;
	delete m_vs;
	delete m_cb;
	delete m_ib;
	delete m_vb;
	delete m_swap_chain;
	// delete m_render_system;
}
