// Copyright (c) 2019 - 2020 PardCode
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#pragma once

#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Matrix4x4.h"
#include "Material.h"


class MiniGame
{
public:
	MiniGame();
	~MiniGame();

	void setWindowSize(const Rect& size);
	TexturePtr& getRenderTarget();

	// Inherited via Window
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();

	// Inherited via InputListener
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);
	virtual void onMouseMove(const Point& mouse_pos);

	virtual void onLeftMouseDown(const Point& mouse_pos);
	virtual void onLeftMouseUp(const Point& mouse_pos);
	virtual void onRightMouseDown(const Point& mouse_pos);
	virtual void onRightMouseUp(const Point& mouse_pos);
	virtual void onMiddleMouseDown(const Point& mouse_pos);
	virtual void onMiddleMouseUp(const Point& mouse_pos);

	void render();
	void update();
	void updateCamera();
	void updateThirdPersonCamera();
	void updateModel(Vector3D position, Vector3D rotation, Vector3D scale, const std::vector<MaterialPtr>& list_materials);
	void updateSkyBox();
	void updateLight();
	void updateSpaceship();

	void drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials);

	void updateViewportProjection();

private:
	SwapChainPtr m_swap_chain;

	TexturePtr m_spaceship_tex;
	MeshPtr m_spaceship_mesh;
	MaterialPtr m_spaceship_mat;

	TexturePtr m_asteroid_tex;
	MeshPtr m_asteroid_mesh;
	MaterialPtr m_asteroid_mat;

	TexturePtr m_sky_tex;
	MeshPtr m_sky_mesh;
	MaterialPtr m_sky_mat;

	MaterialPtr m_base_mat;

private:
	long m_old_delta;
	long m_new_delta;
	float m_delta_time;

	float m_current_cam_distance;
	float m_cam_distance = 14.0f;
	Vector3D m_current_cam_rot;
	Vector3D m_cam_rot;
	Vector3D m_cam_pos;

	float m_spaceship_speed = 125.0f;

	Vector3D m_current_spaceship_pos;
	Vector3D m_spaceship_pos;

	Vector3D m_current_spaceship_rot;
	Vector3D m_spaceship_rot;

	float m_delta_mouse_x = 0.0f;
	float m_delta_mouse_y = 0.0f;

	Vector3D m_asteroids_pos[200];
	Vector3D m_asteroids_rot[200];
	Vector3D m_asteroids_scale[200];

	float m_forward = 0.0f;
	float m_rightward = 0.0f;
	float m_up = 0.0f;

	bool m_turbo_mode = false;
	bool cull = true;
	Matrix4x4 m_world_cam;
	Matrix4x4 m_view_cam;
	Matrix4x4 m_proj_cam;

	float m_time = 0.0f;
	bool m_anim = false; // where this comes from?

	float m_cam_speed = 0.05f;

	Matrix4x4 m_light_rot_matrix;
	bool m_play_state = false;
	bool m_fullscreen_state = false;
	std::vector<MaterialPtr> m_list_materials;
	Vector4D m_light_position;

	TexturePtr m_render_target;
	TexturePtr m_depth_stencil;

	Rect m_window_size;

};
