//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.113
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "RenderTarget.h"


RenderTarget::RenderTarget(ID3D11Device* pDevice):
	m_pDevice(pDevice),
	m_pRenderTargetView(nullptr),
	m_pColorShaderResourceView(nullptr),
	m_pColor(nullptr),
	m_pDepthStencilView(nullptr),
	m_pDepth(nullptr)
{
}

RenderTarget::~RenderTarget(void)
{
	CleanUp();
}

void RenderTarget::CleanUp()
{
	SafeRelease(m_pColor);
	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pColorShaderResourceView);
	SafeRelease(m_pDepth);
	SafeRelease(m_pDepthStencilView);
}

HRESULT RenderTarget::Create(RENDERTARGET_DESC desc)
{
	CleanUp();

	/*if(!desc.IsValid())
		return -1;*/
	// Update PP
	if(desc.IsValid() == -1)
		return -1;

	auto hr = CreateColor(desc);
	if(FAILED(hr))
		return hr;

	hr = CreateDepth(desc);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT RenderTarget::CreateColor(RENDERTARGET_DESC& desc)
{
	auto descValid = desc.IsValid();
	if(descValid == 0)
	{
		//Create Texture2D
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = desc.Width;
		textureDesc.Height = desc.Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1 ;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		auto hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pColor);
		if(FAILED(hr))
			return hr;

		desc.pColor = m_pColor;
	}
	else
	{
		m_pColor = desc.pColor;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	m_pColor->GetDesc(&texDesc);
	desc.Width = texDesc.Width;
	desc.Height = texDesc.Height;
	
	if(texDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		//Create View
		auto hr = m_pDevice->CreateRenderTargetView(m_pColor, NULL, &m_pRenderTargetView);
		if(FAILED(hr))
			return hr;
	}

	if(texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		//Create Shader Resource View 
		auto hr = m_pDevice->CreateShaderResourceView(m_pColor, NULL, &m_pColorShaderResourceView);
		if(FAILED(hr))
			return hr;
	}

	return S_OK;
}

HRESULT RenderTarget::CreateDepth(RENDERTARGET_DESC& desc)
{
	if(desc.pDepth == nullptr)
	{
		//Create Texture2D
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = desc.Width;
		textureDesc.Height = desc.Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDesc.SampleDesc.Count = 1 ;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		auto hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pDepth);
		if(FAILED(hr))
			return hr;

		desc.pDepth = m_pDepth;
	}
	else
	{
		m_pDepth = desc.pDepth;
	}

	//Create DepthStencil View
	auto hr = m_pDevice->CreateDepthStencilView(m_pDepth, NULL, &m_pDepthStencilView);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

// Update PP
void RenderTarget::Clear(const GameContext& gameContext, const FLOAT colorRGBA[4])
{
	gameContext.pDeviceContext->ClearRenderTargetView(GetRenderTargetView(), colorRGBA);
	gameContext.pDeviceContext->ClearDepthStencilView(GetDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}
