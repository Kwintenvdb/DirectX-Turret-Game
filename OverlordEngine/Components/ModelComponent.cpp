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

#include "ModelComponent.h"
#include "..\Base\GeneralStructs.h"
#include "..\Content\ContentManager.h"
#include "..\Graphics\MeshFilter.h"
#include "..\Graphics\Material.h"
#include "..\..\OverlordProject\Materials\FurMaterial.h"
#include "..\Graphics\ModelAnimator.h"


ModelComponent::ModelComponent(wstring assetFile):
m_AssetFile(assetFile),
m_pMeshFilter(nullptr),
m_pMaterial(nullptr),
m_MaterialId(0),
m_MaterialSet(false),
m_pAnimator(nullptr),
m_IgnoreId(false)
{
}


ModelComponent::~ModelComponent()
{
	SafeDelete(m_pAnimator);
	if(m_IgnoreId) SafeDelete(m_pMaterial);
}

void ModelComponent::Initialize(const GameContext& gameContext)
{
	m_pMeshFilter = ContentManager::Load<MeshFilter>(m_AssetFile);
	m_pMeshFilter->BuildIndexBuffer(gameContext);

	if (m_pMeshFilter->m_HasAnimations)
		m_pAnimator = new ModelAnimator(m_pMeshFilter);

	UpdateMaterial(gameContext);
};

inline void ModelComponent::UpdateMaterial(const GameContext& gameContext)
{
	if (m_MaterialSet)
	{
		m_MaterialSet = false;

		if(m_IgnoreId)
		{
			m_pMaterial->Initialize(gameContext);
			m_pMeshFilter->BuildVertexBuffer(gameContext, m_pMaterial);
		}
		else
		{
			auto mat = gameContext.pMaterialManager->GetMaterial(m_MaterialId);
			if (mat == nullptr)
			{
				Logger::LogFormat(LogLevel::Warning, L"ModelComponent::UpdateMaterial > Material with ID \"%i\" doesn't exist!", m_MaterialId);
				return;
			}
			m_pMaterial = mat;
			m_pMaterial->Initialize(gameContext);
			m_pMeshFilter->BuildVertexBuffer(gameContext, m_pMaterial);
		}
	}
}

void ModelComponent::Update(const GameContext& gameContext)
{ 
	UpdateMaterial(gameContext);

	if (m_pAnimator)
		m_pAnimator->Update(gameContext);
};

void ModelComponent::Draw(const GameContext& gameContext)
{ 
	if (!m_pMaterial)
	{
		//Logger::LogWarning(L"ModelComponent::Draw() > No Material!");
		return;
	}

	m_pMaterial->SetEffectVariables(gameContext, this);

	//Set Inputlayout
	gameContext.pDeviceContext->IASetInputLayout(m_pMaterial->GetInputLayout());

	//Set Vertex Buffer
	UINT offset = 0;
	auto vertexBufferData = m_pMeshFilter->GetVertexBufferData(gameContext, m_pMaterial);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);

	//Set Index Buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Primitive Topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DRAW
	auto tech = m_pMaterial->GetTechnique();
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		if(m_pMaterial->GetType() == L"Fur")
		{
			auto mat = reinterpret_cast<FurMaterial*>(m_pMaterial);
			float layers = mat->GetNrOfLayers();
			//cout << "MATERIAL IS FUR " << layers << endl;
			
			if(p == 1)
			{
				for(int i = 0; i < layers; ++i)
				{
					float curr = (float) i / layers; 
					
					mat->SetCurrentLayer(curr);
					mat->SetEffectVariables(gameContext, this);
					tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
					gameContext.pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);						
				}
			}
			else if(p == 0)
			{
				mat->SetEffectVariables(gameContext, this);
				tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
				gameContext.pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);	
			}
		}
		else
		{
			tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
			gameContext.pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);
		}		
	}
};

void ModelComponent::PostDraw(const GameContext& gameContext)
{
	auto tech = m_pMaterial->GetTechnique();
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	if(m_pMaterial->GetType() == L"Fur")
	{
		tech->GetPassByIndex(2)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ModelComponent::SetMaterial(UINT materialId)
{
	m_MaterialSet = true;
	m_IgnoreId = false;
	m_MaterialId = materialId;
}

void ModelComponent::SetMaterial(Material* pMat)
{
	m_IgnoreId = true;
	m_MaterialSet = true;
	m_pMaterial = pMat;
}