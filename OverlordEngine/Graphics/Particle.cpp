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

#include "Particle.h"
#include "../Components/TransformComponent.h"
#include "../Helpers/MathHelper.h"


Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0),
	m_IsActive(false)
{
}


Particle::~Particle(void)
{
}

void Particle::Update(const GameContext& gameContext)
{
	//See Lab10_2

	if(!m_IsActive)
		return;

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();

	if(m_CurrentEnergy < 0)
	{
		m_IsActive = false;
		return;
	}

	m_VertexInfo.Position.x += m_EmitterSettings.Velocity.x * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.y += m_EmitterSettings.Velocity.y * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.z += m_EmitterSettings.Velocity.z * gameContext.pGameTime->GetElapsed();

	m_VertexInfo.Color = m_EmitterSettings.Color;
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2;

	
	m_VertexInfo.Size = m_InitSize + ((m_SizeGrow * m_InitSize) * (1-particleLifePercent));
}

void Particle::Init(XMFLOAT3 initPosition)
{
	//See Lab10_2
	m_IsActive = true;

	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	XMFLOAT3 randomDirection(1,0,0);
	XMVECTOR randomDirectionVec = XMLoadFloat3(&randomDirection);


	XMMATRIX randomRotationMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI),randF(-XM_PI, XM_PI),randF(-XM_PI, XM_PI));
	XMVector3TransformNormal(randomDirectionVec, randomRotationMatrix);

	XMStoreFloat3(&randomDirection, randomDirectionVec);

	float range = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	randomDirection.x *= range;
	randomDirection.y *= range;
	randomDirection.z *= range;
	m_VertexInfo.Position =  XMFLOAT3(initPosition.x + randomDirection.x,initPosition.y + randomDirection.y,initPosition.z + randomDirection.z);
	//m_VertexInfo.Position = initPosition;

	m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_VertexInfo.Size = m_InitSize;

	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	if(m_EmitterSettings.RandomRot) m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
	else m_VertexInfo.Rotation = 0;
}
