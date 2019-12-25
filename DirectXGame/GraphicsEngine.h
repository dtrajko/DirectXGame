#pragma once
#include <d3d11.h>


class SwapChain;
class DeviceContext;


class GraphicsEngine
{

public:
	GraphicsEngine();

	static GraphicsEngine* get();

	// Initialize the GraphicsEngine and DirectX 11 Device
	bool init();

	// Release all resources loaded
	bool release();

	~GraphicsEngine();

	SwapChain* createSwapChain();
	DeviceContext* getImmediateDeviceContext();

private:
	DeviceContext* m_imm_device_context;
	friend class SwapChain;

	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
};
