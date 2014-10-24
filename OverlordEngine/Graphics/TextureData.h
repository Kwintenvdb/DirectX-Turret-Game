//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.82
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once
class TextureData
{
public:
	TextureData(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureShaderResourceView);
	~TextureData(void);

	ID3D11Resource* GetResource() const { return m_pTexture; }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pTextureShaderResourceView; }
	XMFLOAT2 GetDimension() const { return m_Dimension; }

private:

	ID3D11Resource *m_pTexture;
	ID3D11ShaderResourceView *m_pTextureShaderResourceView;
	XMFLOAT2 m_Dimension;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TextureData(const TextureData& t);
	TextureData& operator=(const TextureData& t);
};

