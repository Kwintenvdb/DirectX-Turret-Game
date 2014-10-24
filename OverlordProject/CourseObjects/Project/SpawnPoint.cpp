#include "Base\stdafx.h"

#include "Components\Components.h"
#include "Components/SpriteComponent.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "SpawnPoint.h"
#include "Player.h"
#include "Projectile.h"
#include "MainScene.h"
#include "../../Materials/ColorMaterial.h"
#include "Scenegraph/GameScene.h"

SpawnPoint::SpawnPoint() :
	m_PlayerInRange(false)
{
	SetName("SpawnPoint");
}

SpawnPoint::~SpawnPoint(void)
{

}

void SpawnPoint::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	shared_ptr<PxGeometry> geom(new PxSphereGeometry(25.f));
	auto col = new ColliderComponent(geom, *defaultMaterial);
	col->EnableTrigger(true);
	AddComponent(new RigidBodyComponent(true));	
	AddComponent(col);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receiver, TriggerAction action)
	{
		if(receiver->GetName() == "Player")
		{
			m_PlayerInRange = !m_PlayerInRange;
		}
	});	
}

void SpawnPoint::Update(const GameContext& gameContext)
{

}