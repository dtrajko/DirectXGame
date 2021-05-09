#include "VillageMiniGame.h"

#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "MathUtils.h"
#include "Texture.h"

#include <time.h>


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

VillageMiniGame::VillageMiniGame()
{
}

void VillageMiniGame::render()
{
	// CLEAR THE RENDER TARGET
	// GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0.3f, 0.4f, 1);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_render_target, 0, 0.3f, 0.4f, 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearDepthStencil(this->m_depth_stencil);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRenderTarget(
		this->m_render_target,
		this->m_depth_stencil);

	// SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	// RECT rc = this->getClientWindowRect();
	Rect viewport_size = m_render_target->getSize();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(
		viewport_size.width,
		viewport_size.height);

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

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = 1.0f / 60.0f;
	m_time += m_delta_time;
}

void VillageMiniGame::update()
{
	updateSpaceship();
	updateThirdPersonCamera();
	updateLight();
	updateSkyBox();
}

void VillageMiniGame::updateCamera()
{
	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	m_cam_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_cam_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;

	temp.setIdentity();
	temp.setRotationX(m_cam_rot.m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_cam_rot.m_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() +
		world_cam.getXDirection() * (m_rightward * m_cam_speed) +
		world_cam.getYDirection() * (m_up * m_cam_speed) +
		world_cam.getZDirection() * (m_forward * m_cam_speed);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	updateViewportProjection();
}

void VillageMiniGame::updateThirdPersonCamera()
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
				m_cam_distance = 4.0f;
			}
			else {
				m_cam_distance = 1.0f;
			}
		}
		else {
			if (m_forward > 0.0f) {
				m_cam_distance = 2.0f;
			}
			else {
				m_cam_distance = 1.0f;
			}
		}
	}
	else {
		m_cam_distance = 1.0f;
	}

	m_current_cam_distance = lerp(m_current_cam_distance, m_cam_distance, 2.0f * m_delta_time);

	m_cam_pos = m_current_spaceship_pos;

	Vector3D new_pos = m_cam_pos + world_cam.getZDirection() * (-m_current_cam_distance);
	new_pos = new_pos + world_cam.getYDirection() * 5.0f;

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	m_view_cam = world_cam;

	updateViewportProjection();
}

void VillageMiniGame::updateModel(Vector3D position, Vector3D rotation, Vector3D scale, const std::vector<MaterialPtr>& list_materials)
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

void VillageMiniGame::updateSkyBox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(4000.0f, 4000.0f, 4000.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;

	m_sky_mat->setData(&cc, sizeof(constant));
}

void VillageMiniGame::drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials)
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

void VillageMiniGame::updateViewportProjection()
{
	// int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	// int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	// m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 5000.0f);
}

void VillageMiniGame::updateLight()
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

void VillageMiniGame::updateSpaceship()
{
	Matrix4x4 world_model, temp;
	world_model.setIdentity();

	m_spaceship_rot.m_x += m_delta_mouse_y * m_delta_time * 0.1f;
	m_spaceship_rot.m_y += m_delta_mouse_x * m_delta_time * 0.1f;

	if (m_spaceship_rot.m_x >= 1.57f) {
		m_spaceship_rot.m_x = 1.57f;
	}
	else if (m_spaceship_rot.m_x <= -1.57f) {
		m_spaceship_rot.m_x = -1.57f;
	}

	m_current_spaceship_rot = Vector3D::lerp(m_current_spaceship_rot, m_spaceship_rot, 5.0f * m_delta_time);

	temp.setIdentity();
	temp.setRotationX(m_current_spaceship_rot.m_x);
	world_model *= temp;

	temp.setIdentity();
	temp.setRotationY(m_current_spaceship_rot.m_y);
	world_model *= temp;

	temp.setIdentity();
	temp.setRotationZ(m_current_spaceship_rot.m_z);
	world_model *= temp;

	m_spaceship_speed = 10.0f;
	if (m_turbo_mode) {
		m_spaceship_speed = 30.0f;
	}

	m_spaceship_pos = m_spaceship_pos + world_model.getZDirection() * m_forward * m_spaceship_speed * m_delta_time;
	m_spaceship_pos = m_spaceship_pos + world_model.getXDirection() * m_rightward * m_spaceship_speed * m_delta_time;
	m_spaceship_pos = m_spaceship_pos + world_model.getYDirection() * m_up * m_spaceship_speed * m_delta_time;

	m_current_spaceship_pos = Vector3D::lerp(m_current_spaceship_pos, m_spaceship_pos, 3.0f * m_delta_time);
}

VillageMiniGame::~VillageMiniGame()
{
}

void VillageMiniGame::setWindowSize(const Rect& size)
{
	m_window_size = size;
}

TexturePtr& VillageMiniGame::getRenderTarget()
{
	return m_render_target;
}

void VillageMiniGame::onCreate()
{
	m_play_state = true;


	m_world_cam.setTranslation(Vector3D(0.0f, 1.0f, -2.0f));

	m_list_materials.reserve(32);

	m_render_target = GraphicsEngine::get()->getTextureManager()->createTexture(Rect(1280, 720), Texture::Type::RenderTarget);
	m_depth_stencil = GraphicsEngine::get()->getTextureManager()->createTexture(Rect(1280, 720), Texture::Type::DepthStencil);

	Rect viewport_size = m_render_target->getSize();
	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)1280 / (float)720), 0.1f, 5000.0f);

	m_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/sky.jpg");

	m_sand_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/sand.jpg");

	m_barrel_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/barrel.jpg");
	m_brick_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_brick.jpg");
	m_windows_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_windows.jpg");
	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/house_wood.jpg");

	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/sphere.obj");

	m_terrain_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/terrain.obj");

	m_house_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/house.obj");

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

void VillageMiniGame::onUpdate()
{
	update();
	render();

	m_delta_mouse_x = 0.0f;
	m_delta_mouse_y = 0.0f;
}

void VillageMiniGame::onDestroy()
{
}

void VillageMiniGame::onFocus()
{
}

void VillageMiniGame::onKillFocus()
{
}

void VillageMiniGame::onSize()
{
}

void VillageMiniGame::onKeyDown(int key)
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

void VillageMiniGame::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
	m_up = 0.0f;

	switch (key)
	{
	case VK_SHIFT:
		m_turbo_mode = false;
		break;
	}
}

void VillageMiniGame::onMouseMove(const Point& mouse_pos)
{
	int width = m_window_size.width;
	int height = m_window_size.height;

	m_delta_mouse_x = mouse_pos.m_x - (m_window_size.left + (width / 2.0f));
	m_delta_mouse_y = mouse_pos.m_y - (m_window_size.top + (height / 2.0f));
}

void VillageMiniGame::onLeftMouseDown(const Point& mouse_pos)
{
}

void VillageMiniGame::onLeftMouseUp(const Point& mouse_pos)
{
}

void VillageMiniGame::onRightMouseDown(const Point& mouse_pos)
{
}

void VillageMiniGame::onRightMouseUp(const Point& mouse_pos)
{
}

void VillageMiniGame::onMiddleMouseDown(const Point& mouse_pos)
{
}

void VillageMiniGame::onMiddleMouseUp(const Point& mouse_pos)
{
}
