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
#include "stdafx.h"
#include "TextureData.h"


TextureData::TextureData(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureShaderResourceView):
	m_pTexture(pTexture),
	m_pTextureShaderResourceView(pTextureShaderResourceView)
{
	auto tex2D = static_cast<ID3D11Texture2D*>(pTexture);
	D3D11_TEXTURE2D_DESC text2Ddesc;
	tex2D->GetDesc(&text2Ddesc);

	m_Dimension = XMFLOAT2((float)text2Ddesc.Width, (float)text2Ddesc.Height);
}


TextureData::~TextureData()
{
	SafeRelease(m_pTexture);
	SafeRelease(m_pTextureShaderResourceView);
}
