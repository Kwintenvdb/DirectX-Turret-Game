//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.121
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ParticleEmitterComponent.h"
#include "../Helpers/EffectHelper.h"
#include "../Content/ContentManager.h"
#include "../Content/TextureDataLoader.h"
#include "../Graphics/Particle.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"


ParticleEmitterComponent::ParticleEmitterComponent(const wstring& assetFile, int particleCount):
	m_pEffect(nullptr),
	m_pVertexBuffer(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_Settings(ParticleEmitterSettings()),
	m_AssetFile(assetFile),
	m_pParticleTexture(nullptr),
	m_pWvpVariable(nullptr),
	m_pTextureVariable(nullptr),
	m_pViewInverseVariable(nullptr),
	m_IsActive(true)
{
	//See Lab10_2

	for(int x = 0; x < m_ParticleCount; x++)
		m_Particles.push_back(new Particle(m_Settings));	
}


ParticleEmitterComponent::~ParticleEmitterComponent(void)
{
	//See Lab10_2

	for(int x = 0; x < m_ParticleCount; x++)
	{
		SafeDelete(m_Particles[x]);
	}

	m_Particles.clear();

	m_pInputLayout->Release();
	m_pVertexBuffer->Release();

}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	//See Lab10_2

	LoadEffect(gameContext);

	CreateVertexBuffer(gameContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);

}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	//See Lab10_2

	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");

	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	
	if (!m_pWvpVariable)
	{
		m_pWvpVariable = m_pEffect->GetVariableByName("gWvp")->AsMatrix();
		if (!m_pWvpVariable->IsValid())
		{
			Logger::LogWarning(L"ParticleEmitterComponent::LoadEffectVariables() > \'gWvp\' variable not found!");
			m_pWvpVariable = nullptr;
		}
	}

	if (!m_pViewInverseVariable)
	{
		m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
		if (!m_pViewInverseVariable->IsValid())
		{
			Logger::LogWarning(L"ParticleEmitterComponent::LoadEffectVariables() > \'gViewInverse\' variable not found!");
			m_pViewInverseVariable = nullptr;
		}
	}

	if (!m_pTextureVariable)
	{
		m_pTextureVariable = m_pEffect->GetVariableByName("particleTexture")->AsShaderResource();
		if (!m_pTextureVariable->IsValid())
		{
			Logger::LogWarning(L"ParticleEmitterComponent::LoadEffectVariables() > \'particleTexture\' variable not found!");
			m_pTextureVariable = nullptr;
		}
	}

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);

}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	if(m_pVertexBuffer)SafeRelease(m_pVertexBuffer);

	//Vertexbuffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(ParticleVertex) * m_ParticleCount;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	auto hr = gameContext.pDevice->CreateBuffer(&buffDesc,nullptr,&m_pVertexBuffer);

	Logger::LogHResult(hr, L"ParticleEmitterComponent::BuildVertexBuffer()");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	//See Lab10_2
	float particleInterval =  ((m_Settings.MaxEnergy + m_Settings.MinEnergy)/2) / m_ParticleCount;
	m_LastParticleInit += gameContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;

	for(Particle* particle: m_Particles)
	{
		particle->Update(gameContext);

		if(particle->IsActive())
		{
			pBuffer[++m_ActiveParticles] = particle->GetVertexInfo();
		}
		else
		{
			if(m_LastParticleInit >= particleInterval)
			{
				if(m_IsActive)
				{
					particle->Init(GetTransform()->GetWorldPosition());
					pBuffer[++m_ActiveParticles] = particle->GetVertexInfo();
					m_LastParticleInit = 0;
				}
			}
		}
	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);

}

void ParticleEmitterComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	auto viewProjection = gameContext.pCamera->GetViewProjection();
	m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&(viewProjection)));

	auto viewInverse = gameContext.pCamera->GetViewInverse();
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&(viewInverse)));

	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());


	//Set the input layout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	//Set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Set Vertexbuffer
	UINT offset = 0;
	UINT stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);


	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

int ParticleEmitterComponent::GetActiveParticles()
{
	return m_ActiveParticles;
}

void ParticleEmitterComponent::SetActive(bool active)
{
	m_IsActive = active;
}