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


class SpaceShooterGame : public Window, public InputListener
{
public:
	SpaceShooterGame();
	~SpaceShooterGame();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;
	virtual void onSize() override;

	// Inherited via InputListener
	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point& mouse_pos) override;

	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;
	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;
	virtual void onMiddleMouseDown(const Point& mouse_pos) override;
	virtual void onMiddleMouseUp(const Point& mouse_pos) override;

	void render();
	void update();
	void updateCamera();
	void updateModel(Vector3D position, const std::vector<MaterialPtr>& list_materials);
	void updateSkyBox();
	void updateLight();

	void drawMesh(const MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials);

private:
	SwapChainPtr m_swap_chain;

	TexturePtr m_sky_tex;
	MeshPtr m_sky_mesh;
	MaterialPtr m_sky_mat;
	MaterialPtr m_base_mat;

private:
	long m_old_delta;
	long m_new_delta;
	float m_delta_time;

	float m_rot_x = 0.0f;
	float m_rot_y = 0.0f;

	float m_light_rot_y = 0.0f;

	float m_forward = 0.0f;
	float m_rightward = 0.0f;
	float m_up = 0.0f;

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

};
