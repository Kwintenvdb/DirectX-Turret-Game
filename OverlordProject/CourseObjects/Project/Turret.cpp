#include "Base\stdafx.h"

#include "Components\Components.h"
#include "Components/SpriteComponent.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "Turret.h"
#include "Player.h"
#include "Projectile.h"
#include "MainScene.h"
#include "Enemy.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "Scenegraph/GameScene.h"

Turret::Turret() :
	m_pCube(nullptr),
	m_Timer(1),
	m_DeltaTime(),
	m_PlayerInRange(false),
	m_CanShoot(false),
	m_Damage(10),
	m_PlayerTimer(),
	m_StartPlayerTimer(false),
	m_PlayerOwned(false),
	m_HP(nullptr),
	m_pMC(nullptr),
	m_pColMat(nullptr),
	m_Target(nullptr),
	m_Targets()
{
	SetName("Turret");
}

Turret::~Turret(void)
{

}

void Turret::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	m_pColMat = new ColorMaterial();

	// COLLIDER
	float colliderRange = 40;
	shared_ptr<PxGeometry> geom(new PxSphereGeometry(colliderRange));
	auto col = new ColliderComponent(geom, *defaultMaterial);
	col->EnableTrigger(true);
	AddComponent(new RigidBodyComponent(true));	
	AddComponent(col);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receiver, TriggerAction action)
	{
		if(receiver->GetName() == "Player")
		{
			auto player = reinterpret_cast<Player*>(receiver);
			auto pos = GetTransform()->GetPosition().y;
			auto ppos = receiver->GetTransform()->GetPosition().y;
			if(abs(pos - ppos) < 15)
			{
				if(action == TriggerAction::ENTER)
				{
					m_PlayerInRange = true;
					if(m_PlayerOwned) player->SetInvulnerable(true);
				}
				else m_PlayerInRange = false;

				if(m_PlayerInRange) m_StartPlayerTimer = true;
				else m_StartPlayerTimer = false;
			}		
		}
		else if(receiver->GetName() == "Enemy")
		{
			m_StartPlayerTimer = false;
			auto enemy = reinterpret_cast<Enemy*>(receiver);
			if(m_PlayerOwned)
			{
				if(action == TriggerAction::ENTER)
				{
					m_Targets++;
				}
				else
				{
					m_Targets--;
				}
			}
		}
	});	

	// turret model
	auto modelObj = new GameObject();
	auto model = new ModelComponent(L"./Resources/Meshes/Turret.ovm");
	model->SetMaterial(m_pColMat);
	modelObj->AddComponent(model);
	modelObj->GetTransform()->Scale(0.25f, 0.25f, 0.25f);
	shared_ptr<PxGeometry> cgeom(new PxBoxGeometry(4.5f,15,4.5f));
	AddComponent(new RigidBodyComponent(true));
	AddComponent(new ColliderComponent(cgeom, *defaultMaterial, PxTransform(0,7.5f,0)));
	AddChild(modelObj);

	// plane
	auto m2 = new GameObject();
	m_pMC = new ModelComponent(L"./Resources/Meshes/plane.ovm");

	m_pMC->SetMaterial(TurrNorm);
	m2->AddComponent(m_pMC);
	AddChild(m2);
	float scale = 8.5f;
	m2->GetTransform()->Scale(scale,1,scale);
	m2->GetTransform()->Translate(0,0.05f,0);
}

void Turret::Update(const GameContext& gameContext)
{
	m_DeltaTime = gameContext.pGameTime->GetElapsed();

	ShootPlayer();
	ManageOwnership();
	if(m_PlayerOwned)
	{
		m_pMC->SetMaterial(TurrOwned);
	}
}

void Turret::ShootPlayer()
{	
	if(m_PlayerInRange && !m_PlayerOwned)
	{
		m_pMC->SetMaterial(TurrAggro);
		m_Timer += m_DeltaTime;
		if(m_Timer >= 0.9f)
		{
			m_Timer = 0;
			m_CanShoot = true;
		}
		else
		{
			m_CanShoot = false;
		}
	}
	else if(!m_PlayerOwned)
	{
		m_pMC->SetMaterial(TurrNorm);
		m_Timer = 0.5f;
	}

	// shoot enemy
	if(m_PlayerOwned && m_Targets > 0)
	{
		m_Timer += m_DeltaTime;
		if(m_Timer >= 0.5f)
		{
			m_Timer = 0;
			m_CanShoot = true;
		}
		else
		{
			m_CanShoot = false;
		}
	}
	else if(m_PlayerOwned && m_Targets <= 0)
	{
		m_Timer = 0.2f;
		m_CanShoot = false;
	}
}

void Turret::ManageOwnership()
{
	float maxTime(8.f), decrease(0.7f);

	// increase
	if(m_StartPlayerTimer && m_PlayerInRange)
	{
		m_PlayerTimer += m_DeltaTime;
	}

	// decrease
	if(!m_StartPlayerTimer && !m_PlayerOwned)
	{
		m_PlayerTimer -= m_DeltaTime * decrease;
	}
	
	Clamp<float>(m_PlayerTimer, maxTime, 0.f);
	
	if(m_PlayerTimer >= maxTime)
	{
		m_PlayerOwned = true;
	}
	

	// COLORS
	XMFLOAT4 red(1,0,0,1), green(0,1,0,1), col;
	float percent = m_PlayerTimer / maxTime;

	auto vR = XMLoadFloat4(&red);
	auto vG = XMLoadFloat4(&green);
	auto lerp = XMVectorLerp(vR, vG, percent);
	XMStoreFloat4(&col, lerp);

	m_pColMat->SetColor(col);
}

void Turret::FindClosestEnemy()
{
	if(m_PlayerOwned)
	{
		Enemy* closest = nullptr;
		float dist = 100000;
		for(Enemy* en : m_Enemies)
		{
			if(en != nullptr)
			{
				XMFLOAT3 enPos = en->GetTransform()->GetPosition();
				XMFLOAT3 dir = GetTransform()->GetPosition();
				dir.x -= enPos.x;
				dir.y -= enPos.y;
				dir.z -= enPos.z;

				XMFLOAT3 length;
				XMStoreFloat3(&length, XMVector3Length(XMLoadFloat3(&dir)));
				if(length.x <= dist)
				{
					dist = length.x;
					closest = en;
				}
			}
		}

		if(dist <= 40)
		{
			m_Target = closest;
		}
		else m_Target = nullptr;
	}
}