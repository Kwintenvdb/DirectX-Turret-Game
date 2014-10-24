#include "Base\stdafx.h"

#include "Components\Components.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "Pickup.h"
#include "Player.h"
#include "Projectile.h"
#include "MainScene.h"
#include "../../Materials/ColorMaterial.h"
#include "Scenegraph/GameScene.h"

Pickup::Pickup(string name) :
	m_pCube(nullptr),
	m_Timer(),
	m_DeltaTime(),
	m_IsPickedUp(false)
{
	SetName(name);
	m_Rotation = XMFLOAT3(0,0,90);
}

Pickup::~Pickup(void)
{

}

void Pickup::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	auto particleObj = new GameObject();
	auto particleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/glowOwn2.png", 20);
	particleEmitter->SetVelocity(XMFLOAT3(0, 2.5f, 0));
	particleEmitter->SetMinSize(.15f);
	particleEmitter->SetMaxSize(.25f);
	particleEmitter->SetMinEnergy(2.5f);
	particleEmitter->SetMaxEnergy(5.5f);
	particleEmitter->SetMinSizeGrow(15.f);
	particleEmitter->SetMaxSizeGrow(20.f);
	particleEmitter->SetMinEmitterRange(-1.5f);
	particleEmitter->SetMaxEmitterRange(1.5f);
	particleEmitter->SetColor(XMFLOAT4(1.f, 1.f, 1.f, 0.75f));
	particleEmitter->SetActive(true);
	particleObj->AddComponent(particleEmitter);
	AddChild(particleObj);
	particleObj->GetTransform()->Translate(2,6.f,0);

	// speedup model
	if(m_Name == "SpeedUp")
	{
		auto mObj = new GameObject();
		auto mC = new ModelComponent(L"./Resources/Meshes/speedup.ovm");
		mC->SetMaterial(PickMat);
		mObj->AddComponent(mC);
		mObj->GetTransform()->Scale(0.175f,0.175f,0.175f);
		mObj->GetTransform()->Translate(5,0,0);
		AddChild(mObj);
	}
	else // invulnerable
	{
		auto mObj = new GameObject();
		auto mC = new ModelComponent(L"./Resources/Meshes/shield.ovm");
		mC->SetMaterial(PickMat);
		mObj->AddComponent(mC);
		mObj->GetTransform()->Scale(0.3f,0.3f,0.3f);
		mObj->GetTransform()->Rotate(0,90,0);
		mObj->GetTransform()->Translate(7.f,0,0);		
		AddChild(mObj);
	}

	shared_ptr<PxGeometry> geom(new PxSphereGeometry(7));
	auto col = new ColliderComponent(geom, *defaultMaterial);
	col->EnableTrigger(true);
	auto rb = new RigidBodyComponent();
	rb->SetKinematic(true);
	AddComponent(rb);	
	AddComponent(col);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receiver, TriggerAction action)
	{
		if(receiver->GetName() == "Player")
		{
			cout << "PICKUP: PLAYER HIT" << endl;
			auto player = reinterpret_cast<Player*>(receiver);
			m_IsPickedUp = true;
			OnPickup(player);
		}
	});	
}

void Pickup::Update(const GameContext& gameContext)
{
	m_DeltaTime = gameContext.pGameTime->GetElapsed();

	float speed = 82.5f;
	m_Rotation.y += speed * m_DeltaTime;
	GetTransform()->Rotate(m_Rotation);
}

void Pickup::OnPickup(Player* player)
{
	if(m_Name == "SpeedUp")
	{
		cout << "SpeedUp picked up" << endl;
		player->SpeedUp(2.75f);
	}
	else player->SetInvulnerable(true);
}