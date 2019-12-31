#pragma once

#include "Window.h"
#include "RenderSystem.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Matrix4x4.h"


class AppWindow: public Window, public InputListener
{
public:
	AppWindow();
	void update();
	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;

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

private:
	RenderSystem* m_render_system;
	SwapChain* m_swap_chain;
	VertexBuffer* m_vb;
	IndexBuffer* m_ib;
	ConstantBuffer* m_cb;
	VertexShader* m_vs;
	PixelShader* m_ps;

	// Timer
	float m_old_delta;
	float m_new_delta;
	float m_delta_time;
	float m_delta_pos;
	float m_delta_rot;
	float m_delta_scale;

	float m_rot_x = 0.0f;
	float m_rot_y = 0.0f;

	float m_scale_cube = 1;

	float m_forward = 0.0f;
	float m_right = 0.0f;
	float m_up = 0.0f;
	Matrix4x4 m_world_cam;

	float cam_speed = 0.02f;
	float mouse_speed = 0.2f;

	float width;
	float height;
};
