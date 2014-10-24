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

#include "FurMaterial.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"

ID3DX11EffectShaderResourceVariable* FurMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pColorSRV = nullptr;
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pFinSRV = nullptr;
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pNoiseSRV = nullptr;
ID3DX11EffectScalarVariable* FurMaterial::m_pCurrentLayerSRV = nullptr;
ID3DX11EffectScalarVariable* FurMaterial::m_pMaxHairLengthSRV = nullptr;
ID3DX11EffectVectorVariable* FurMaterial::m_pCombSRV = nullptr;
ID3DX11EffectScalarVariable* FurMaterial::m_pDensityVar = nullptr;
ID3DX11EffectScalarVariable* FurMaterial::m_pFinOpacityVar = nullptr;

FurMaterial::FurMaterial(float maxHairLength, float nrOfLayers) : 
	Material(L"./Resources/Effects/Fur.fx"),
	m_pDiffuseTexture(nullptr),
	m_pColorTexture(nullptr),
	m_pFinTexture(nullptr),
	m_pNoiseTexture(nullptr),
	m_MaxHairLength(maxHairLength),
	m_CurrentLayer(),
	m_NrOfLayers(nrOfLayers),
	m_Density(6.f),
	m_FinOpacity(0.5f)
{
	SetType(L"Fur");
}


FurMaterial::~FurMaterial()
{

}

void FurMaterial::SetFurTexture(const wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void FurMaterial::SetColorTexture(const wstring& assetFile)
{
	m_pColorTexture = ContentManager::Load<TextureData>(assetFile);
}

void FurMaterial::SetFinTexture(const wstring& assetFile)
{
	m_pFinTexture = ContentManager::Load<TextureData>(assetFile);
}

void FurMaterial::SetNoiseTexture(const wstring& assetFile)
{
	m_pNoiseTexture = ContentManager::Load<TextureData>(assetFile);
}

void FurMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("FurTexture")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'FurTexture\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	if(!m_pCurrentLayerSRV)
	{
		m_pCurrentLayerSRV = m_pEffect->GetVariableByName("CurrentLayer")->AsScalar();
		if (!m_pCurrentLayerSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'CurrentLayer\' variable not found!");
			m_pCurrentLayerSRV = nullptr;
		}
	}

	if(!m_pMaxHairLengthSRV)
	{
		m_pMaxHairLengthSRV = m_pEffect->GetVariableByName("MaxHairLength")->AsScalar();
		if (!m_pMaxHairLengthSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'MaxHairLength\' variable not found!");
			m_pMaxHairLengthSRV = nullptr;
		}
	}

	if(!m_pColorSRV)
	{
		m_pColorSRV = m_pEffect->GetVariableByName("ColTexture")->AsShaderResource();
		if (!m_pColorSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'ColTexture\' variable not found!");
			m_pColorSRV = nullptr;
		}
	}

	if(!m_pFinSRV)
	{
		m_pFinSRV = m_pEffect->GetVariableByName("FinTexture")->AsShaderResource();
		if (!m_pFinSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'FinTexture\' variable not found!");
			m_pFinSRV = nullptr;
		}
	}

	if(!m_pCombSRV)
	{
		m_pCombSRV = m_pEffect->GetVariableByName("Comb")->AsVector();
		if (!m_pCombSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'Comb\' variable not found!");
			m_pCombSRV = nullptr;
		}
	}

	if(!m_pNoiseSRV)
	{
		m_pNoiseSRV = m_pEffect->GetVariableByName("NoiseTexture")->AsShaderResource();
		if (!m_pNoiseSRV->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'NoiseTexture\' variable not found!");
			m_pNoiseSRV = nullptr;
		}
	}

	if(!m_pDensityVar)
	{
		m_pDensityVar = m_pEffect->GetVariableByName("Density")->AsScalar();
		if (!m_pDensityVar->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'Density\' variable not found!");
			m_pDensityVar = nullptr;
		}
	}

	if(!m_pFinOpacityVar)
	{
		m_pFinOpacityVar = m_pEffect->GetVariableByName("MaxOpacity")->AsScalar();
		if (!m_pFinOpacityVar->IsValid())
		{
			Logger::LogWarning(L"FurMaterial::LoadEffectVariables() > \'MaxOpacity\' variable not found!");
			m_pFinOpacityVar = nullptr;
		}
	}
}

void FurMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if(m_pColorTexture && m_pColorSRV)
		m_pColorSRV->SetResource(m_pColorTexture->GetShaderResourceView());

	if(m_pFinTexture && m_pFinSRV)
		m_pFinSRV->SetResource(m_pFinTexture->GetShaderResourceView());

	if(m_pNoiseTexture && m_pNoiseSRV)
		m_pNoiseSRV->SetResource(m_pNoiseTexture->GetShaderResourceView());

	if(m_pMaxHairLengthSRV)
		m_pMaxHairLengthSRV->SetFloat(m_MaxHairLength);

	if(m_pCurrentLayerSRV)
		m_pCurrentLayerSRV->SetFloat(m_CurrentLayer);

	if(m_pCombSRV)
	{
		float comb[3] = { m_Comb.x, m_Comb.y, m_Comb.z };
		m_pCombSRV->SetFloatVector(comb);
	}

	if(m_pDensityVar)
		m_pDensityVar->SetFloat(m_Density);

	if(m_pFinOpacityVar)
		m_pFinOpacityVar->SetFloat(m_FinOpacity);
}
