#include "Base\stdafx.h"

#include "Projectile.h"
#include "Player.h"
#include "Enemy.h"
#include "Components\Components.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"

Projectile::Projectile(float size, int damage, XMFLOAT3 destination, float speed, XMFLOAT4 color) :
	m_Color(color),
	m_Size(size),
	m_Speed(speed),
	m_Damage(damage),
	m_Destination(destination),
	m_pSphere(nullptr),
	m_RigidBody(nullptr),
	m_PlayerHit(false),
	m_CanDestroy(false),
	m_Timer(0)
{
	SetName("Projectile");
}


Projectile::~Projectile(void)
{
}

void Projectile::Initialize(const GameContext& gameContext)
{
	m_pSphere = new SpherePrefab(m_Size,10, m_Color);
	AddChild(m_pSphere);

	auto physX = PhysxManager::GetInstance()->GetPhysics();	
	// bouncy
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	m_RigidBody = new RigidBodyComponent();
	m_RigidBody->SetKinematic(true);
	AddComponent(m_RigidBody);
	std::shared_ptr<PxGeometry> sphereGeom(new PxSphereGeometry(m_Size));
	auto col = new ColliderComponent(sphereGeom, *defaultMaterial, PxTransform(PxQuat(XM_PIDIV2,PxVec3(0,0,1))));
	col->EnableTrigger(true);
	AddComponent(col);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receiver, TriggerAction action)
	{
		if(receiver->GetName() == "Player")
		{
			cout << "PROJECTILE: PLAYER HIT" << endl;
			m_PlayerHit = true;
			auto player = reinterpret_cast<Player*>(receiver);
			if(!player->IsInvulnerable())
			{
				player->Damage(m_Damage);				
			}
		}
		else if(receiver->GetName() == "Enemy")
		{
			cout << "PROJECTILE: ENEMY HIT" << endl;
			m_PlayerHit = true;
			auto enemy = reinterpret_cast<Enemy*>(receiver);
			enemy->Damage(m_Damage);
		}
		else if(receiver->GetName() == "Terrain")
		{
			m_CanDestroy = true;
		}
	});	
}

void Projectile::PostInitialize(const GameContext& gameContext)
{
	XMFLOAT3 pos = GetTransform()->GetWorldPosition();
	m_Direction = m_Destination;
	m_Direction.x -= pos.x;
	m_Direction.y -= pos.y;
	m_Direction.z -= pos.z;
}

void Projectile::Update(const GameContext& gameContext)
{
	float deltaTime = gameContext.pGameTime->GetElapsed();
	m_Timer += deltaTime;
	//cout << m_Timer << endl;
	if(m_Timer >= 3) m_CanDestroy = true;

	XMFLOAT3 pos = GetTransform()->GetWorldPosition();
	
	auto vecDir = XMLoadFloat3(&m_Direction);
	XMVECTOR nDir = XMVector3Normalize(vecDir);
	
	XMVECTOR newPos = XMLoadFloat3(&pos) + (nDir * m_Speed * deltaTime);
	GetTransform()->Translate(newPos);
}

void Projectile::Shoot(const PxVec3& force, const GameContext& gameContext)
{
	m_RigidBody->AddForce(force, PxForceMode::eIMPULSE);
}

bool Projectile::CanDestroy()
{
	return m_CanDestroy;
}