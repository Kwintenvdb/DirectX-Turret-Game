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
#pragma once

struct RENDERTARGET_DESC
{
public:
	int Width;
	int Height;
	ID3D11Texture2D* pColor;
	ID3D11Texture2D* pDepth;

	RENDERTARGET_DESC():
		Width(-1),
		Height(-1),
		pColor(nullptr),
		pDepth(nullptr)
	{}

	int IsValid()
	{
		if(pColor == nullptr)
		{
			if(Width <= 0 || Height <= 0)
				return -1;
			else
				return 0;
		}

		return 1;
	}
};

class RenderTarget
{
public:
	RenderTarget(ID3D11Device* pDevice);
	~RenderTarget(void);

	HRESULT Create(RENDERTARGET_DESC desc);
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
	bool HasRenderTargetView() const { return m_pRenderTargetView != nullptr; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pColorShaderResourceView; }
	bool HasShaderResourceView() const { return m_pColorShaderResourceView != nullptr; }

	// Update PP
	void Clear(const GameContext& gameContext, const FLOAT colorRGBA[4]);

private:
	
	ID3D11Device* m_pDevice;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11ShaderResourceView* m_pColorShaderResourceView;
	ID3D11Texture2D* m_pColor;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepth;

	HRESULT CreateColor(RENDERTARGET_DESC& desc);
	HRESULT CreateDepth(RENDERTARGET_DESC& desc);
	void CleanUp();

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	RenderTarget( const RenderTarget &obj);
	RenderTarget& operator=( const RenderTarget& obj);
};

