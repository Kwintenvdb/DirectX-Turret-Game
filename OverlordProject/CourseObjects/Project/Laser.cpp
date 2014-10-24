#include "Base\stdafx.h"
#include "Laser.h"
#include "Components\Components.h"
#include "Components\ParticleEmitterComponent.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"

Laser::Laser(XMFLOAT3 vel) :
	m_Timer(),
	m_Velocity(vel)
{

}

Laser::~Laser(void)
{

}

void Laser::Initialize(const GameContext& gameContext)
{
	auto particleObj = new GameObject();
	auto particleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/laser.png", 85);
	particleEmitter->SetVelocity(m_Velocity);
	particleEmitter->SetMinSize(1.75f);
	particleEmitter->SetMaxSize(1.75f);
	particleEmitter->SetMinEnergy(0.75f);
	particleEmitter->SetMaxEnergy(0.75f);
	particleEmitter->SetMinSizeGrow(1.f);
	particleEmitter->SetMaxSizeGrow(1.f);
	particleEmitter->SetMinEmitterRange(0);
	particleEmitter->SetMaxEmitterRange(0);
	particleEmitter->SetColor(XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	particleEmitter->SetRandomRot(false);
	particleEmitter->SetActive(true);
	particleObj->AddComponent(particleEmitter);
	AddChild(particleObj);	
}

void Laser::Update(const GameContext& gameContext)
{
	m_Timer += gameContext.pGameTime->GetElapsed();
	if(m_Timer > 0.15f)
	{
		auto em = GetComponent<ParticleEmitterComponent>(true);
		em->SetActive(false);
	}
}