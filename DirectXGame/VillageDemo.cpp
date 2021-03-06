// Copyright (c) 2019 - 2020 PardCode
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include "VillageDemo.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "MathUtils.h"


struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	Vector4D m_light_direction;
	Vector4D m_camera_position;
	Vector4D m_light_position = Vector4D(0.0f, 1.0f, 0.0f, 0.0f);
	float m_light_radius = 4.0f;
	float m_time = 0.0f;
};

VillageDemo::VillageDemo()
{
}

void VillageDemo::render()
{
	// CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0.3f, 0.4f, 1);
	// SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	// Render Terrain
	m_list_materials.clear();
	m_list_materials.push_back(m_terrain_mat);
	updateModel(Vector3D(0, 0, 0), Vector3D(), Vector3D(1, 1, 1), m_list_materials);
	drawMesh(m_terrain_mesh, m_list_materials);

	// Render House
	m_list_materials.clear();
	m_list_materials.push_back(m_barrel_mat);
	m_list_materials.push_back(m_brick_mat);
	m_list_materials.push_back(m_windows_mat);
	m_list_materials.push_back(m_wood_mat);

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			updateModel(Vector3D(10.0f * i, 0.0f, 10.0f * j), Vector3D(), Vector3D(1, 1, 1), m_list_materials);
			drawMesh(m_house_mesh, m_list_materials);
		}
	}

	// Render Skybox/sphere
	m_list_materials.clear();
	m_list_materials.push_back(m_sky_mat);
	drawMesh(m_sky_mesh, m_list_materials);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = 1.0f / 60.0f;
	m_time += m_delta_time;
}

void VillageDemo::update()
{
	updatePlayer();
	updateThirdPersonCamera();
	updateLight();
	updateSkyBox();
}

void VillageDemo::updateThirdPersonCamera()
{
	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	m_cam_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_cam_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;

	if (m_cam_rot.m_x >= 1.57f) {
		m_cam_rot.m_x = 1.57f;
	}
	else if (m_cam_rot.m_x <= -1.57f) {
		m_cam_rot.m_x = -1.57f;
	}

	m_current_cam_rot = Vector3D::lerp(m_current_cam_rot, m_cam_rot, 3.0f * m_delta_time);

	temp.setIdentity();
	temp.setRotationX(m_current_cam_rot.m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_cam_rot.m_y);
	world_cam *= temp;

	if (m_forward) {
		if (m_turbo_mode) {
			if (m_forward > 0.0f) {
				m_cam_distance = 5.0f;
			}
			else {
				m_cam_distance = 1.0f;
			}
		}
		else {
			if (m_forward > 0.0f) {
				m_cam_distance = 4.0f;
			}
			else {
				m_cam_distance = 2.0f;
			}
		}
	}
	else {
		m_cam_distance = 3.0f;
	}

	m_current_cam_distance = lerp(m_current_cam_distance, m_cam_distance, 2.0f * m_delta_time);

	m_cam_pos = m_current_player_pos;

	Vector3D new_pos = m_cam_pos + world_cam.getZDirection() * (-m_current_cam_distance);
	new_pos = new_pos + world_cam.getYDirection() * 5.0f;

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 5000.0f);
}

void VillageDemo::updateModel(Vector3D position, Vector3D rotation, Vector3D scale, const std::vector<MaterialPtr>& list_materials)
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

	cc.m_light_position = m_light_position;
	cc.m_light_radius = 0.0f;
	cc.m_light_direction = m_light_rot_matrix.getZDirection();
	cc.m_time = m_time;

	for (size_t m = 0; m < list_materials.size(); m++)
	{
		list_materials[m]->setData(&cc, sizeof(constant));
	}
}

void VillageDemo::updateSkyBox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(1000.0f, 1000.0f, 1000.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;

	m_sky_mat->setData(&cc, sizeof(constant));
}

void VillageDemo::updateLight()
{
	Matrix4x4 temp;
	m_light_rot_matrix.setIdentity();

	temp.setIdentity();
	temp.setRotationX(-0.707f);
	m_light_rot_matrix *= temp;

	temp.setIdentity();
	temp.setRotationY(0.707f);
	m_light_rot_matrix *= temp;
}

void VillageDemo::updatePlayer()
{
	Matrix4x4 world_model, temp;
	world_model.setIdentity();

	m_player_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_player_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;

	if (m_player_rot.m_x >= 1.57f) {
		m_player_rot.m_x = 1.57f;
	}
	else if (m_player_rot.m_x <= -1.57f) {
		m_player_rot.m_x = -1.57f;
	}

	m_current_player_rot = Vector3D::lerp(m_current_player_rot, m_player_rot, 5.0f * m_delta_time);

	temp.setIdentity();
	temp.setRotationX(m_current_player_rot.m_x);
	world_model *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_player_rot.m_y);
	world_model *= temp;

	temp.setIdentity();
	temp.setRotationZ(m_current_player_rot.m_z);
	world_model *= temp;

	m_player_speed = 5.0f;
	if (m_turbo_mode) {
		m_player_speed = 20.0f;
	}

	m_player_pos = m_player_pos + world_model.getZDirection() * m_forward * m_player_speed * m_delta_time;
	m_player_pos = m_player_pos + world_model.getXDirection() * m_rightward * m_player_speed * m_delta_time;
	m_player_pos = m_player_pos + world_model.getYDirection() * m_up * m_player_speed * m_delta_time;

	m_current_player_pos = Vector3D::lerp(m_current_player_pos, m_player_pos, 3.0f * m_delta_time);
}

void VillageDemo::drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials)
{
	// SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());
	// SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());

	for (size_t m = 0; m < mesh->getNumMaterialSlots(); m++)
	{
		if (m >= list_materials.size()) break;

		MaterialSlot mat = mesh->getMaterialSlot(m);
		GraphicsEngine::get()->setMaterial(list_materials[m]);

		// FINALLY DRAW TRIANGLES
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList((UINT)mat.num_indices, 0, (UINT)mat.start_index);
	}
}

void VillageDemo::updateViewportProjection()
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 5000.0f);
}

VillageDemo::~VillageDemo()
{
}

void VillageDemo::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);

	m_play_state = true;
	InputSystem::get()->showCursor(false);

	m_sky_tex     = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/sky.jpg");

	m_sand_tex    = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/sand.jpg");

	m_barrel_tex  = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/barrel.jpg");
	m_brick_tex   = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_brick.jpg");
	m_windows_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_windows.jpg");
	m_wood_tex    = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_wood.jpg");

	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/sphere.obj");

	m_terrain_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/terrain.obj");

	m_house_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/house.obj");

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_mat = GraphicsEngine::get()->createMaterial(L"PointLightVertexShader.hlsl", L"PointLightPixelShader.hlsl");
	m_mat->addTexture(m_wall_tex);
	m_mat->setCullMode(CULL_MODE_BACK);

	m_terrain_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_terrain_mat->addTexture(m_sand_tex);
	m_terrain_mat->setCullMode(CULL_MODE_BACK);

	m_barrel_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_barrel_mat->addTexture(m_barrel_tex);
	m_barrel_mat->setCullMode(CULL_MODE_BACK);

	m_brick_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_brick_mat->addTexture(m_brick_tex);
	m_brick_mat->setCullMode(CULL_MODE_BACK);

	m_windows_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_windows_mat->addTexture(m_windows_tex);
	m_windows_mat->setCullMode(CULL_MODE_BACK);

	m_wood_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_wood_mat->addTexture(m_wood_tex);
	m_wood_mat->setCullMode(CULL_MODE_BACK);

	m_sky_mat = GraphicsEngine::get()->createMaterial(L"PointLightVertexShader.hlsl", L"SkyBoxPixelShader.hlsl");
	m_sky_mat->addTexture(m_sky_tex);
	m_sky_mat->setCullMode(CULL_MODE_FRONT);

	m_world_cam.setTranslation(Vector3D(0.0f, 1.0f, -2.0f));

	m_list_materials.reserve(32);
}

void VillageDemo::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	update();
	render();

	m_delta_mouse_x = 0.0f;
	m_delta_mouse_y = 0.0f;
}

void VillageDemo::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->setFullScreen(false, 1, 1);
}

void VillageDemo::onFocus()
{
	InputSystem::get()->addListener(this);
}

void VillageDemo::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void VillageDemo::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right - rc.left, rc.bottom - rc.top);

	update();
	render();
}

void VillageDemo::onKeyDown(int key)
{
	switch (key)
	{
	case 'W':
		m_forward = 1.0f;
		break;
	case 'S':
		m_forward = -1.0f;
		break;
	case 'A':
		m_rightward = -1.0f;
		break;
	case 'D':
		m_rightward = 1.0f;
		break;
	case 'Q':
		m_up = -1.0f;
		break;
	case 'E':
		m_up = 1.0f;
		break;
	case VK_SHIFT:
		m_turbo_mode = true;
		break;
	}
}

void VillageDemo::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
	m_up = 0.0f;

	switch (key)
	{
	case VK_ESCAPE:
		if (m_play_state) {
			m_play_state = false;
			InputSystem::get()->showCursor(!m_play_state);
		}
		break;
	case 'F':
		m_fullscreen_state = m_fullscreen_state ? false : true;
		RECT size_screen = this->getSizeScreen();
		m_swap_chain->setFullScreen(m_fullscreen_state, size_screen.right, size_screen.bottom);
		break;
	case VK_SHIFT:
		m_turbo_mode = false;
		break;
	}
}

void VillageDemo::onMouseMove(const Point & mouse_pos)
{
	if (!m_play_state) return;

	RECT win_size = this->getClientWindowRect();

	int width = (win_size.right - win_size.left);
	int height = (win_size.bottom - win_size.top);

	m_delta_mouse_x = (int)(mouse_pos.m_x - (win_size.left + (width / 2.0f)));
	m_delta_mouse_y = (int)(mouse_pos.m_y - (win_size.top + (height / 2.0f)));

	InputSystem::get()->setCursorPosition(Point(win_size.left + (int)(width / 2.0f), win_size.top + (int)(height / 2.0f)));
}

void VillageDemo::onLeftMouseDown(const Point & mouse_pos)
{
	if (!m_play_state) {
		m_play_state = true;
		InputSystem::get()->showCursor(!m_play_state);
	}
}

void VillageDemo::onLeftMouseUp(const Point & mouse_pos)
{
}

void VillageDemo::onRightMouseDown(const Point & mouse_pos)
{
}

void VillageDemo::onRightMouseUp(const Point & mouse_pos)
{
}

void VillageDemo::onMiddleMouseDown(const Point& mouse_pos)
{
}

void VillageDemo::onMiddleMouseUp(const Point& mouse_pos)
{
}
