/*MIT License

C++ 3D Game Tutorial Series (https://github.com/PardCode/CPP-3D-Game-Tutorial-Series)

Copyright (c) 2019-2021, PardCode

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/


#include "VillageMainApp.h"

#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "MathUtils.h"

#include <time.h>


__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	Vector4D m_light_direction;
	Vector4D m_camera_position;
	Vector4D m_light_position = Vector4D(0, 1, 0, 0);
	float m_light_radius = 10.0f;
	float m_time = 0.0f;

};

VillageMainApp::VillageMainApp()
{
}

void VillageMainApp::render()
{
	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0.3f, 0.4f, 1);

	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	m_list_materials.clear();
	m_list_materials.push_back(m_monitor_mat);
	m_list_materials.push_back(m_screen_mat);
	updateModel(Vector3D(0, 0, 0), Vector3D(0, 3.14f, 0), Vector3D(1, 1, 1), m_list_materials);
	drawMesh(m_monitor_mesh, m_list_materials);

	//RENDER SKYBOX/SPHERE
	m_list_materials.clear();
	m_list_materials.push_back(m_sky_mat);
	drawMesh(m_sky_mesh, m_list_materials);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearDepthStencil(this->m_swap_chain);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = 0.0166666666f;
	m_time += m_delta_time;
}

void VillageMainApp::update()
{
	updateThirdPersonCamera();
	updateLight();
	updateSkyBox();
}

void VillageMainApp::updateModel(Vector3D position, Vector3D rotation, Vector3D scale, const std::vector<MaterialPtr>& list_materials)
{
	constant cc;

	Matrix4x4 temp;
	cc.m_world.setIdentity();

	temp.setIdentity();
	temp.setScale(scale);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(rotation.m_x);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(rotation.m_y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationZ(rotation.m_z);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setTranslation(position);
	cc.m_world *= temp;

	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;
	cc.m_camera_position = m_world_cam.getTranslation();

	cc.m_light_position = m_light_rot_matrix.getTranslation();
	cc.m_light_radius = 0.0f;
	cc.m_light_direction = m_light_rot_matrix.getZDirection();
	cc.m_time = m_time;

	for (unsigned int m = 0; m < list_materials.size(); m++)
	{
		list_materials[m]->setData(&cc, sizeof(constant));
	}
}

void VillageMainApp::updateThirdPersonCamera()
{
	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	m_cam_rot.m_x += m_delta_mouse_y * 0.001f;
	m_cam_rot.m_y += m_delta_mouse_x * 0.001f;

	if (m_cam_rot.m_x >= 1.57f)
		m_cam_rot.m_x = 1.57f;
	else if (m_cam_rot.m_x <= -1.57f)
		m_cam_rot.m_x = -1.57f;

	m_current_cam_rot = Vector3D::lerp(m_current_cam_rot, m_cam_rot, 3.0f * m_delta_time);

	temp.setIdentity();
	temp.setRotationX(m_current_cam_rot.m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_cam_rot.m_y);
	world_cam *= temp;

	m_current_cam_distance = lerp(m_current_cam_distance, m_cam_distance, 2.0f * m_delta_time);

	m_cam_pos = Vector3D(0.0f, 0.16f, 0.0f);

	Vector3D new_pos = m_cam_pos + world_cam.getZDirection() * (-m_current_cam_distance);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	updateViewportProjection();
}

void VillageMainApp::updateSkyBox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(4000.0f, 4000.0f, 4000.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;

	m_sky_mat->setData(&cc, sizeof(constant));
}

void VillageMainApp::updateLight()
{
	Matrix4x4 temp;
	m_light_rot_matrix.setIdentity();

	temp.setIdentity();
	temp.setRotationX(-0.707f);
	m_light_rot_matrix *= temp;

	temp.setIdentity();
	temp.setRotationY(2.14f);
	m_light_rot_matrix *= temp;
}

void VillageMainApp::drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials)
{
	for (unsigned int m = 0; m < list_materials.size(); m++)
	{
		if (m == list_materials.size()) break;

		MaterialSlot mat = mesh->getMaterialSlot(m);

		GraphicsEngine::get()->setMaterial(list_materials[m]);
		//SET THE VERTICES OF THE TRIANGLE TO DRAW
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());
		//SET THE INDICES OF THE TRIANGLE TO DRAW
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());
		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList((UINT)mat.num_indices, 0, (UINT)mat.start_index);
	}
}

void VillageMainApp::updateViewportProjection()
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 5000.0f);
}

VillageMainApp::~VillageMainApp()
{
}

void VillageMainApp::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);

	m_play_state = true;
	InputSystem::get()->showCursor(false);

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/stars_map.jpg");
	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/sphere.obj");

	m_monitor_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/monitor.obj");

	m_brick_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/brick_d.jpg");
	m_brick_normal_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/brick_n.jpg");

	m_base_mat = GraphicsEngine::get()->createMaterial(L"DirectionalLightVertexShader.hlsl", L"DirectionalLightPixelShader.hlsl");
	m_base_mat->setCullMode(CULL_MODE_BACK);

	m_sky_mat = GraphicsEngine::get()->createMaterial(L"SkyBoxVertexShader.hlsl", L"SkyBoxPixelShader.hlsl");
	m_sky_mat->addTexture(m_sky_tex);
	m_sky_mat->setCullMode(CULL_MODE_FRONT);

	m_brick_mat = GraphicsEngine::get()->createMaterial(L"DirLightBumpVS.hlsl", L"DirLightBumpPS.hlsl");
	m_brick_mat->addTexture(m_brick_tex);
	m_brick_mat->addTexture(m_brick_normal_tex);
	m_brick_mat->setCullMode(CULL_MODE_BACK);

	m_monitor_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_monitor_mat->addTexture(m_brick_tex);
	m_monitor_mat->setCullMode(CULL_MODE_BACK);

	m_screen_mat = GraphicsEngine::get()->createMaterial(m_base_mat);
	m_screen_mat->setCullMode(CULL_MODE_BACK);

	m_world_cam.setTranslation(Vector3D(0.0f, 0.0f, -2.0f));

	m_list_materials.reserve(32);

	m_mini_game.setWindowSize(Rect(rc.right - rc.left, rc.bottom - rc.top));
	m_mini_game.onCreate();

	m_screen_mat->addTexture(m_mini_game.getRenderTarget());
}

void VillageMainApp::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	m_mini_game.onUpdate();

	this->update();
	this->render();

	m_delta_mouse_x = 0;
	m_delta_mouse_y = 0;
}

void VillageMainApp::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->setFullScreen(false, 1, 1);
}

void VillageMainApp::onFocus()
{
	InputSystem::get()->addListener(this);
}

void VillageMainApp::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void VillageMainApp::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right - rc.left, rc.bottom - rc.top);
	m_mini_game.setWindowSize(Rect(rc.right - rc.left, rc.bottom - rc.top));
	this->update();
	this->render();
}

void VillageMainApp::onKeyDown(int key)
{
	if (!m_play_state)
	{
		m_mini_game.onKeyDown(key);
	}
}

void VillageMainApp::onKeyUp(int key)
{
	if (!m_play_state)
	{
		m_mini_game.onKeyUp(key);
	}

	if (key == VK_ESCAPE)
	{
		if (m_play_state)
		{
			m_play_state = false;
		}
		else if (!m_play_state)
		{
			m_play_state = true;
		}
	}
	else if (key == 'F')
	{
		m_fullscreen_state = (m_fullscreen_state) ? false : true;
		RECT size_screen = this->getSizeScreen();

		m_swap_chain->setFullScreen(m_fullscreen_state, size_screen.right, size_screen.bottom);
	}
}

void VillageMainApp::onMouseMove(const Point& mouse_pos)
{
	RECT win_size = this->getClientWindowRect();

	int width = (win_size.right - win_size.left);
	int height = (win_size.bottom - win_size.top);

	if (m_play_state)
	{
		m_delta_mouse_x = (float)(mouse_pos.m_x - (win_size.left + (width / 2.0f)));
		m_delta_mouse_y = (float)(mouse_pos.m_y - (win_size.top + (height / 2.0f)));
	}

	InputSystem::get()->setCursorPosition(Point(win_size.left + (int)(width / 2.0f), win_size.top + (int)(height / 2.0f)));

	if (!m_play_state)
	{
		m_mini_game.onMouseMove(mouse_pos);
	}
}

void VillageMainApp::onLeftMouseDown(const Point& mouse_pos)
{
}

void VillageMainApp::onLeftMouseUp(const Point& mouse_pos)
{
}

void VillageMainApp::onRightMouseDown(const Point& mouse_pos)
{
}

void VillageMainApp::onRightMouseUp(const Point& mouse_pos)
{
}

void VillageMainApp::onMiddleMouseDown(const Point& mouse_pos)
{
}

void VillageMainApp::onMiddleMouseUp(const Point& mouse_pos)
{
}
