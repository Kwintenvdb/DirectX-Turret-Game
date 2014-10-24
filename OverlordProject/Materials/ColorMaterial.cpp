//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.81
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "Base\stdafx.h"

#include "ColorMaterial.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"

ID3DX11EffectVectorVariable* ColorMaterial::m_pColorVar = nullptr;

ColorMaterial::ColorMaterial(bool enableTransparency) : 
	Material(L"./Resources/Effects/PosNormCol3D.fx", enableTransparency?L"TransparencyTech":L"")
{
	m_Color = XMFLOAT4(1,0,0,1);
}


ColorMaterial::~ColorMaterial()
{
}

void ColorMaterial::LoadEffectVariables()
{
	if(!m_pColorVar)
	{
		m_pColorVar = m_pEffect->GetVariableByName("gColor")->AsVector();
		if(!m_pColorVar->IsValid())
		{
			Logger::LogWarning(L"ColorMaterial::LoadEffectVariables() > \'gColor\' variable not found!");
			m_pColorVar = nullptr;
		}
	}
}

void ColorMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if(m_pColorVar)
	{
		float color[4] =
		{
			m_Color.x,
			m_Color.y,
			m_Color.z,
			m_Color.w
		};
	
		m_pColorVar->SetFloatVector((float*)&color);
	}
}
