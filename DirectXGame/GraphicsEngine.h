#pragma once
#include <d3d11.h>


class SwapChain;
class DeviceContext;
class VertexBuffer;


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
	VertexBuffer* createVertexBuffer();

	bool createShaders();
	bool setShaders();
	void getShaderBufferAndSize(void** bytecode, UINT* size);

private:
	DeviceContext* m_imm_device_context;

	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
	ID3D11DeviceContext* m_imm_context;

	ID3DBlob* m_vsblob = nullptr;
	ID3DBlob* m_psblob = nullptr;
	ID3D11VertexShader* m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;

private:
	friend class SwapChain;
	friend class VertexBuffer;

};
