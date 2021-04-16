#include "Texture.h"

#include "GraphicsEngine.h"
#include "RenderSystem.h"

#include <DirectXTex.h>


Texture::Texture()
{
}

Texture::Texture(const wchar_t* full_path) : Resource(full_path)
{
	DirectX::ScratchImage image_data;
	HRESULT res = DirectX::LoadFromWICFile(full_path, DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image_data);

	if (SUCCEEDED(res))
	{
		res = DirectX::CreateTexture(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, image_data.GetImages(),
			image_data.GetImageCount(), image_data.GetMetadata(), &m_texture);

		if (FAILED(res)) throw std::exception("Texture not created successfully.");

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = image_data.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = (UINT)image_data.GetMetadata().mipLevels;
		desc.Texture2D.MostDetailedMip = 0;

		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = (FLOAT)image_data.GetMetadata().mipLevels;

		res = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateSamplerState(&sampler_desc, &m_sampler_state);
		if (FAILED(res)) throw std::exception("Texture SamplerState not created successfully.");

		res = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture, &desc, &m_shader_res_view);
		if (FAILED(res)) throw std::exception("Texture ShaderResourceView not created successfully.");
	}
	else
	{
		throw std::exception("Texture not created successfully.");
	}
}

Texture::Texture(const Rect& size, Texture::Type type) : Resource(L"")
{
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = size.width;
	tex_desc.Height = size.height;

	if (type == Normal)
	{
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else if (type == RenderTarget)
	{
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	}
	else if (type == DepthStencil)
	{
		tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	}

	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.MipLevels = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.ArraySize = 1;
	tex_desc.CPUAccessFlags = 0;

	auto hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&tex_desc, nullptr, (ID3D11Texture2D**)&m_texture);

	if (FAILED(hr))
	{
		throw std::exception("Texture not created successfully (Texture2D).");
	}

	if (type == Normal || type == RenderTarget)
	{
		hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture, NULL, &m_shader_res_view);
		if (FAILED(hr))
		{
			throw std::exception("Texture not created successfully (ShaderResourceView).");
		}
	}

	if (type == RenderTarget)
	{
		hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRenderTargetView(m_texture, NULL, &m_render_target_view);
		if (FAILED(hr))
		{
			throw std::exception("Texture not created successfully (RenderTargetView).");
		}
	}
	else if (type == DepthStencil)
	{
		hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateDepthStencilView(m_texture, NULL, &m_depth_stencil_view);
		if (FAILED(hr))
		{
			throw std::exception("Texture not created successfully (DepthStencilView).");
		}
	}
}

Texture::~Texture()
{
	if (m_render_target_view) m_render_target_view->Release();
	if (m_depth_stencil_view) m_depth_stencil_view->Release();
	if (m_shader_res_view) m_shader_res_view->Release();
	if (m_sampler_state) m_sampler_state->Release();
	if (m_texture) m_texture->Release();
}

Rect Texture::getSize()
{
	return m_size;
}

Texture::Type Texture::getType()
{
	return m_type;
}
