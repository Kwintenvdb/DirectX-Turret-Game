#include "Base\stdafx.h"
#include "Fire.h"
#include "Components\Components.h"
#include "Components\ParticleEmitterComponent.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"

Fire::Fire(XMFLOAT3 vel) :
	m_IsLit(false),
	m_Timer(),
	m_Velocity(vel)
{

}

Fire::~Fire(void)
{

}

void Fire::Initialize(const GameContext& gameContext)
{
	auto particleObj = new GameObject();
	auto particleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Flames.png", 35);
	particleEmitter->SetVelocity(m_Velocity);
	particleEmitter->SetMinSize(3);
	particleEmitter->SetMaxSize(4);
	particleEmitter->SetMinEnergy(0.55f);
	particleEmitter->SetMaxEnergy(.85f);
	particleEmitter->SetMinSizeGrow(1.f);
	particleEmitter->SetMaxSizeGrow(1.5f);
	particleEmitter->SetMinEmitterRange(-1);
	particleEmitter->SetMaxEmitterRange(1);
	particleEmitter->SetColor(XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	particleEmitter->SetActive(true);
	particleObj->AddComponent(particleEmitter);
	AddChild(particleObj);
}

void Fire::Update(const GameContext& gameContext)
{
	m_Timer += gameContext.pGameTime->GetElapsed();
	if(m_Timer > 0.15f)
	{
		auto em = GetComponent<ParticleEmitterComponent>(true);
		em->SetActive(false);
	}
}