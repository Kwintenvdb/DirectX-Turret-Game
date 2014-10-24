#include "Base\stdafx.h"

#include "Components\Components.h"
#include "Graphics\ModelAnimator.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "Enemy.h"
#include "Player.h"
#include "Fire.h"
#include "../../Materials/ColorMaterial.h"

Enemy::Enemy(Player* player) :
	m_pPlayer(player),
	m_pCube(nullptr),
	m_CanMove(true),
	m_CanShoot(false),
	m_CanPushBack(false),
	m_AddFire(false),
	m_Timer(),
	m_FallVelocity(),
	m_Health(100),
	m_Angle(),
	m_ShootTimer(),
	m_DistanceToPlayer(),
	m_pModelComp(nullptr)
{
	SetName("Enemy");
}

Enemy::~Enemy(void)
{
}

void Enemy::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	auto modelObj = new GameObject();
	m_pModelComp = new ModelComponent(L"./Resources/Meshes/player.ovm");
	m_pModelComp->SetMaterial(EnemyMat);
	modelObj->AddComponent(m_pModelComp);
	modelObj->GetTransform()->Scale(0.3f, 0.3f, 0.3f);
	modelObj->GetTransform()->Translate(0,-5.5f,0);
	modelObj->GetTransform()->Rotate(0,90,0);
	AddChild(modelObj);

	m_Controller = new ControllerComponent(defaultMaterial, 3, 6);
	m_Controller->SetCollisionGroup(CollisionGroupFlag::Group1);
	AddComponent(m_Controller);
}

void Enemy::Update(const GameContext& gameContext)
{
	auto animator = m_pModelComp->GetAnimator();
	animator->SetAnimationSpeed(0.5f);

	if(m_AddFire)
	{
		auto fire = new Fire();
		AddChild(fire);
		fire->GetTransform()->Scale(2,2,2);
		m_AddFire = false;
	}

	float maxDist = 100;
	m_ShootTimer += gameContext.pGameTime->GetElapsed();
	if(m_ShootTimer > 2.25f && m_DistanceToPlayer < maxDist)
	{
		m_CanShoot = true;
		m_ShootTimer = 0;
	}
	else m_CanShoot = false;
}

void Enemy::MoveToPlayer(const GameContext& gameContext)
{
	XMFLOAT3 playerPos = m_pPlayer->GetTransform()->GetWorldPosition();
	float deltaTime = gameContext.pGameTime->GetElapsed();	
	float jumpAccel = 100;
	auto controller = GetComponent<ControllerComponent>();

	if(controller->GetCollisionFlags() == PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_FallVelocity = 0;
	}
	else 
	{
		m_FallVelocity -= jumpAccel * deltaTime;
	}

	if(m_CanMove)
	{
		float speed = 15.5f;

		XMFLOAT3 pos = GetTransform()->GetWorldPosition();
		XMFLOAT3 dir = playerPos;
		dir.x -= pos.x;
		dir.y -= pos.y;
		dir.z -= pos.z;

		XMFLOAT3 length;
		XMStoreFloat3(&length, XMVector3Length(XMLoadFloat3(&dir)));
		m_DistanceToPlayer = length.x;

		float angle = atan2f(dir.z, dir.x);
		angle = angle * (180.f/XM_PI);
		if(angle < 0)
		{
		    angle = 360 - (-angle);
		}

		auto vecDir = XMLoadFloat3(&dir);
		auto vecNormDir = XMVector3Normalize(vecDir);

		XMFLOAT3 normDir;
		XMStoreFloat3(&normDir, vecNormDir * speed * deltaTime);
		normDir.y = m_FallVelocity;

		GetTransform()->Rotate(0, 270 - angle, 0);
		m_Controller->Move(normDir);
	}
}