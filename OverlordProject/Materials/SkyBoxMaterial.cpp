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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "Base\stdafx.h"

#include "SkyBoxMaterial.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"



ID3DX11EffectShaderResourceVariable* SkyBoxMaterial::m_pCubeMapSRVvariable = nullptr;

SkyBoxMaterial::SkyBoxMaterial() : Material(L"./Resources/Effects/SkyBox.fx"),
	m_pCubeMap(nullptr)
{
}


SkyBoxMaterial::~SkyBoxMaterial()
{

}

void SkyBoxMaterial::SetDiffuseTexture(const wstring& assetFile)
{
	m_pCubeMap = ContentManager::Load<TextureData>(assetFile);
}

void SkyBoxMaterial::LoadEffectVariables()
{
	if (!m_pCubeMapSRVvariable)
	{
		m_pCubeMapSRVvariable = m_pEffect->GetVariableByName("m_CubeMap")->AsShaderResource();
		if (!m_pCubeMapSRVvariable->IsValid())
		{
			Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'m_CubeMap\' variable not found!");
			m_pCubeMapSRVvariable = nullptr;
		}
	}
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pCubeMap && m_pCubeMapSRVvariable)
	{
		m_pCubeMapSRVvariable->SetResource(m_pCubeMap->GetShaderResourceView());
	}
}
