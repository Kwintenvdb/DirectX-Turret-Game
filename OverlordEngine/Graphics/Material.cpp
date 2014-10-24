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
#include "stdafx.h"

#include "Material.h"
#include <algorithm>
#include "../Content/ContentManager.h"
#include "../Diagnostics/Logger.h"
#include "../Base/GeneralStructs.h"
#include <math.h>

#include "../Components/CameraComponent.h"
#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"
#include "../Base/MaterialManager.h"

Material::Material(const wstring& effectFile, const wstring& technique) :
m_pEffect(nullptr),
m_pInputLayout(nullptr),
m_pWorldMatrixVariable(0),
m_pViewMatrixVariable(0),
m_pViewInverseMatrixVariable(0),
m_pWvpMatrixVariable(0),
m_effectFile(effectFile),
m_pInputLayoutSize(0),
m_IsInitialized(false),
m_InputLayoutID(0),
m_TechniqueName(technique)
{
}


Material::~Material(void)
{
	SafeRelease(m_pInputLayout);
	m_pInputLayoutDescriptions.clear();
}

void Material::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		LoadEffect(gameContext, m_effectFile);
		m_IsInitialized = true;
	}
}

bool Material::LoadEffect(const GameContext& gameContext, wstring effectFile)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);

	if (m_TechniqueName.size() > 0)
	{
		auto techString = string(m_TechniqueName.begin(), m_TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}
	

	//Build InputLayout
	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);


	auto effectVar = m_pEffect->GetVariableBySemantic("World");
	m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("View");
	m_pViewMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("ViewInverse");
	m_pViewInverseMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("WorldViewProjection");
	m_pWvpMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;

	LoadEffectVariables();

	return true;
}

void Material::SetEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_IsInitialized)
	{
		auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
		auto view = XMLoadFloat4x4(&gameContext.pCamera->GetView());
		auto projection = XMLoadFloat4x4(&gameContext.pCamera->GetProjection());

		if (m_pWorldMatrixVariable)
			m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
		
		if (m_pViewMatrixVariable)
			m_pViewMatrixVariable->SetMatrix(reinterpret_cast<float*>(&view));

		if (m_pWvpMatrixVariable)
		{
			XMMATRIX wvp = world*view*projection;
			m_pWvpMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&(wvp)));
		}

		if (m_pViewInverseMatrixVariable)
		{
			auto viewInv = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
			m_pViewInverseMatrixVariable->SetMatrix(reinterpret_cast<float*>(&viewInv));
		}

		UpdateEffectVariables(gameContext, pModelComponent);
	}
}
