#include "Base\stdafx.h"

#include "Enemy.h"
#include "Components\Components.h"
#include "Graphics\ModelAnimator.h"
#include "Physx/PhysxManager.h"
#include "Player.h"
#include "Diagnostics\Logger.h"
#include "../../Materials/DiffuseMaterial.h"
#include "Scenegraph/GameScene.h"
#include "Physx/PhysxProxy.h"
#include "Fire.h"
#include "Laser.h"

enum GameInput
{
	MOVEUP = 0,
	MOVEDOWN = 1,
	MOVELEFT = 2,
	MOVERIGHT = 3,
	SHOOT = 4,
	DEBUG = 5
};

Player::Player() :
	m_CamObj(nullptr),
	m_HP(nullptr),
	m_Health(150),
	m_pController(nullptr),
	m_pModelComp(nullptr),
	m_Velocity(0,0,0),
	m_Gravity(9.81f),
	m_JumpAcceleration(m_Gravity / 0.1f),
	m_JumpVelocity(),
	m_RunVelocity(32.5f),
	m_RotationSpeed(20),
	m_TotalYaw(),
	m_TotalPitch(),
	m_Invulnerable(false),
	m_InvulTimer(),
	m_SpeedUp(1),
	m_DamageTimer(),
	m_CamRot(17.5f),
	m_HeadAnim(false),
	m_LastDamaged(),
	m_AddFire(false),
	m_ShootTimer()
{
}

Player::~Player(void)
{

}

void Player::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(.1f, .1f, .1f);

	m_pController = new ControllerComponent(bouncyMaterial, 3, 6);
	AddComponent(m_pController);

	m_CamObj = new GameObject();
	m_CamObj->GetTransform()->Translate(0,20,-45);
	m_CamObj->GetTransform()->Rotate(m_CamRot,0,0);
	auto camera = new CameraComponent();
	m_CamObj->AddComponent(camera);
	AddChild(m_CamObj);

	auto modelObj = new GameObject();
	m_pModelComp = new ModelComponent(L"./Resources/Meshes/player.ovm");
	m_pModelComp->SetMaterial(PlayerMat);
	modelObj->AddComponent(m_pModelComp);
	auto trans = m_pModelComp->GetTransform();
	float scale = .3f;
	trans->Scale(scale, scale, scale);
	trans->Translate(0,-5.5f,0);
	trans->Rotate(0,270,0);
	AddChild(modelObj);
}

void Player::Update(const GameContext& gameContext)
{
	if(m_Health > 0)
	{
	auto animator = m_pModelComp->GetAnimator();
	animator->SetAnimationSpeed(0.5f * m_SpeedUp);
	animator->SetPlayReversed(false);

	float deltaTime = gameContext.pGameTime->GetElapsed();

	// blur effect
	if(m_DamageTimer > 0)
	{
		m_DamageTimer -= 0.55f * deltaTime;
	}
	Clamp<float>(m_DamageTimer, 0.2f, 0);

	// healing
	float healTime = 4.5f, healAmount = 3.25f;
	m_LastDamaged += deltaTime;
	if(m_LastDamaged >= healTime)
	{
		m_Health += healAmount * deltaTime;
	}
	Clamp<float>(m_Health,150,0);

	// raycasting
	m_ShootTimer += deltaTime;
	if(gameContext.pInput->IsActionTriggered(GameInput::SHOOT) && m_ShootTimer > 0.4f)
	{
		Raycast(gameContext);
		m_ShootTimer = 0;
	}

	// invulnerability
	if(m_Invulnerable)
	{
		m_InvulTimer += deltaTime;
		if(m_InvulTimer >= 10)
		{
			m_Invulnerable = false;
			m_InvulTimer = 0;
		}
	}

	/////
	XMFLOAT2 look = XMFLOAT2(0,0);
	float lookSpeed = 4.f;
	//if(gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
	//{
	//	auto mouseMove = gameContext.pInput->GetMouseMovement();
	//	//gameContext.pInput->
	//	look.x = static_cast<float>(mouseMove.x);
	//	look.y = static_cast<float>(mouseMove.y);
	//}
	if(gameContext.pInput->IsKeyboardKeyDown(VK_RIGHT))
		look.x = lookSpeed;
	if(gameContext.pInput->IsKeyboardKeyDown(VK_LEFT))
		look.x = -lookSpeed;
	if(gameContext.pInput->IsKeyboardKeyDown(VK_UP))
		look.y = lookSpeed * 0.75f;
	if(gameContext.pInput->IsKeyboardKeyDown(VK_DOWN))
		look.y = -lookSpeed * 0.75f;

	if(look.x == 0 && look.y == 0)
	{
		look = gameContext.pInput->GetThumbstickPosition(false);
	}

	m_TotalYaw += look.x * m_RotationSpeed * deltaTime;
	m_TotalPitch += look.y * m_RotationSpeed * deltaTime;
	m_CamRot += look.y * m_RotationSpeed * deltaTime;
	GetTransform()->Rotate(0, m_TotalYaw, 0);
	Clamp<float>(m_CamRot, 25, -10);
	XMFLOAT3 camPos = m_CamObj->GetTransform()->GetPosition();
	m_CamObj->GetTransform()->Translate(camPos.x, 7.5f + m_CamRot * 0.75f, camPos.z);
	m_CamObj->GetTransform()->Rotate(m_CamRot, 0, 0);

	// Reset velocity
	m_Velocity = XMFLOAT3(0,m_Velocity.y,0);

	if(m_pController->GetCollisionFlags() == PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_Velocity.y = 0;
		m_JumpVelocity = 0;
	}
	else 
	{
		m_JumpVelocity -= m_JumpAcceleration * deltaTime;
	}

	XMFLOAT3 forward = GetTransform()->GetForward();
	XMFLOAT3 right = GetTransform()->GetRight();
	bool isMoving = false;

#pragma region "input"
	if(gameContext.pInput->IsActionTriggered(GameInput::MOVEUP))
	{
		isMoving = true;
		m_Velocity.x += forward.x * m_RunVelocity * m_SpeedUp;
		m_Velocity.z += forward.z * m_RunVelocity * m_SpeedUp;
	}

	if(gameContext.pInput->IsActionTriggered(GameInput::MOVEDOWN))
	{
		isMoving = true;
		m_Velocity.x -= forward.x * m_RunVelocity * m_SpeedUp;
		m_Velocity.z -= forward.z * m_RunVelocity * m_SpeedUp;
		animator->SetPlayReversed(true);
	}
	
	float sideMove = 0.85f;

	if(gameContext.pInput->IsActionTriggered(GameInput::MOVERIGHT))
	{
		isMoving = true;
		m_Velocity.x += right.x * m_RunVelocity * sideMove * m_SpeedUp;
		m_Velocity.z += right.z * m_RunVelocity * sideMove * m_SpeedUp;
	}
	
	if(gameContext.pInput->IsActionTriggered(GameInput::MOVELEFT))
	{
		isMoving = true;
		m_Velocity.x -= right.x * m_RunVelocity * sideMove * m_SpeedUp;
		m_Velocity.z -= right.z * m_RunVelocity * sideMove * m_SpeedUp;
	}
#pragma endregion

	if(!isMoving)
	{
		animator->Pause();
		m_Velocity.x = 0.0f;
		m_Velocity.z = 0.0f;
	}
	else
	{
		animator->Play();
	}

	XMFLOAT3 tempVelocity = m_Velocity;
	tempVelocity.y += m_JumpVelocity;

	XMFLOAT3 delta = tempVelocity;
	delta.x = tempVelocity.x * deltaTime;
	delta.y = tempVelocity.y * deltaTime;
	delta.z = tempVelocity.z * deltaTime;

	m_pController->Move(delta);

	// Speed up
	if(m_SpeedUp > 1)
	{
		m_SpeedUp -= 0.0715f * deltaTime;
	}
	if(m_SpeedUp < 1)
	{
		m_SpeedUp = 1;
	}

	// Add fire
	if(m_AddFire)
	{
		auto fire = new Fire();
		AddChild(fire);
		fire->GetTransform()->Scale(2,2,2);
		m_Fires.push_back(fire);
		m_AddFire = false;
	}
	// Delete fires
	//Fire* toDel = nullptr;
	//for(Fire* fire : m_Fires)
	//	if(fire)
	//		if(fire->IsDone())
	//			toDel = fire;
	//if(toDel)
	//{
	//	for(auto it = m_Fires.begin(); it != m_Fires.end();)
	//	{
	//		if(toDel == *it)
	//		{
	//			RemoveChild(*it);
	//			it = m_Fires.erase(it); 
	//		}
	//		else
	//			++it;
	//	}
	//}
	}
}							   

void Player::Draw(const GameContext& gameContext)
{

}

void Player::Raycast(const GameContext& gameContext)
{
	GameScene* scene = GetScene();

	XMFLOAT3 pos = GetTransform()->GetPosition();
	XMFLOAT3 fw = GetTransform()->GetForward();
	PxVec3 rayOrigin(pos.x,pos.y + 1.f,pos.z), rayDirection(fw.x,fw.y,fw.z);
	rayOrigin.x += fw.x * 2.5f;
	rayOrigin.z += fw.z * 2.5f;

	const PxU32 bufSize = 20;
	PxRaycastHit hit[bufSize];
	PxRaycastBuffer buf(hit, bufSize); // [out] Blocking and touching hits will be stored here

	if(scene->GetPhysxProxy()->Raycast(rayOrigin, rayDirection, 5000, buf))
	{
		for(PxU32 i = 0; i < buf.nbTouches; ++i)
		{
			BaseComponent* component = static_cast<BaseComponent*>(buf.touches[i].actor->userData);
			GameObject* go = component->GetGameObject();
			string name = go->GetName();	
			cout << "RAYCAST OBJECT: " << name << endl;		

			if(name == "Enemy")
			{
				Enemy* enemy = reinterpret_cast<Enemy*>(go);
				int dmg = 12.5f;
				enemy->Damage(dmg);
			}
		}

		PxVec3 vel = rayDirection * 1000;
		auto laser = new Laser(XMFLOAT3(vel.x, vel.y, vel.z));
		AddChild(laser);
	}
}

void Player::Damage(float amount)
{
	m_Health -= amount; 
	m_DamageTimer = 0.1f; 
	m_LastDamaged = 0;

	m_AddFire = true;
}