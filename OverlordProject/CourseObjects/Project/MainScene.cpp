#include "Base\stdafx.h"

#include "MainScene.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"
#include "Base/OverlordGame.h"

#include "Projectile.h"
#include "Player.h"
#include "Enemy.h"
#include "Fire.h"
#include "Turret.h"
#include "Pickup.h"
#include "LevelPiece.h"
#include "SpawnPoint.h"
#include "Terrain.h"
#include "ResourceBar.h"

#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Helpers\PhysxHelper.h"
#include "Scenegraph\SceneManager.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/SkinnedDiffuseMaterial.h"
#include "../../Materials/FurMaterial.h"
#include "../../Materials/BlurMaterial.h"
#include "../../Materials/GrayMaterial.h"
#include "Helpers/MathHelper.h"
#include "Content/ContentManager.h"
#include "Graphics\TextRenderer.h"
#include <memory>

enum GameInput
{
	MOVEUP = 0,
	MOVEDOWN = 1,
	MOVELEFT = 2,
	MOVERIGHT = 3,
	SHOOT = 4,
	DEBUG = 5,
	QUIT = 6
};

MainScene::MainScene(void):
	GameScene(L"MainScene"),
	m_Terrain(nullptr),
	m_HP(nullptr),
	m_Player(nullptr),
	m_pEnemy(nullptr),
	m_Timer(),
	m_pSpriteFont(nullptr),
	m_Debug(false),
	m_pBlurMat(nullptr),
	m_pGrayMat(nullptr),
	m_MatRemoved(true),
	m_HpBar(nullptr),
	m_SpeedBar(nullptr),
	m_InvBar(nullptr),
	m_BlurIt(),
	m_pMatPlayer(nullptr),
	m_DeadScreen(nullptr),
	m_WinScreen(nullptr),
	m_Won(false)
{
}


MainScene::~MainScene(void)
{
	m_Enemies.clear();
	m_Fires.clear();
	m_Turrets.clear();
	m_Projectiles.clear();
	m_SpawnPoints.clear();
	m_Pickups.clear();
}

void MainScene::Initialize(const GameContext& gameContext)
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	DebugRenderer::ToggleDebugRenderer();
	
	m_pGrayMat = new GrayMaterial();
	AddPostProcessingEffect(m_pGrayMat);

	// CREATING STUFF
	m_pSpriteFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");
	CreateInputs(gameContext);	
	CreateMaterials(gameContext);
	CreateTerrain(gameContext);
	CreateTurrets(gameContext);
	CreatePickups();
	CreateSpawnPoints();

	auto skyBox = new SkyBoxPrefab();
	skyBox->GetTransform()->Rotate(0,130,0);
	AddChild(skyBox);

	// PLAYER
	m_Player = new Player();
	m_Player->SetName("Player");
	AddChild(m_Player);
	m_Player->GetTransform()->Translate(0,5,-125);
	m_Player->GetTransform()->Rotate(0,180,0);

	SetActiveCamera(m_Player->GetComponent<CameraComponent>(true));

	// RESOURCE BARS
	m_HpBar		= new ResourceBar(L"hpbar", L"hptext", 6.25f);
	m_SpeedBar	= new ResourceBar(L"speedbar", L"speedtext", 3.75f);
	m_InvBar	= new ResourceBar(L"invulbar", L"invtext", 1.25f);
	AddChild(m_HpBar);
	AddChild(m_SpeedBar);
	AddChild(m_InvBar);

	// DEAD
	float x = OverlordGame::GetGameSettings().Window.Width / 2;
	float y = OverlordGame::GetGameSettings().Window.Height / 2;

	m_DeadScreen = new GameObject();
	auto sp = new SpriteComponent(L"./Resources/Textures/dead.png",XMFLOAT2(0.5f,0.5f),XMFLOAT4(1,1,1,0));
	m_DeadScreen->AddComponent(sp);
	AddChild(m_DeadScreen);
	m_DeadScreen->GetTransform()->Translate(x,y,0);

	m_WinScreen = new GameObject();
	auto sp2 = new SpriteComponent(L"./Resources/Textures/win.png",XMFLOAT2(0.5f,0.5f),XMFLOAT4(1,1,1,0));
	m_WinScreen->AddComponent(sp2);
	AddChild(m_WinScreen);
	m_WinScreen->GetTransform()->Translate(x,y,0);
}

void MainScene::Update(const GameContext& gameContext)
{
	float exp = 0.75f;
	float d = (1 - (m_Player->GetHealth() / 100)) * exp;
	Clamp<float>(d,1,0);
	m_pGrayMat->SetDesaturation(d);

	ManageHUD(gameContext);
	ManageProjectiles(gameContext);

	if(m_Won)
	{
		auto sp = m_WinScreen->GetComponent<SpriteComponent>();
		float a = sp->GetColor().w;
		a = a + gameContext.pGameTime->GetElapsed() * (1 - a);
		sp->SetColor(XMFLOAT4(1,1,1,a));

		if(gameContext.pInput->IsActionTriggered(GameInput::QUIT))
		{
			PostQuitMessage(0);
		}
	}

	if(m_Player->GetHealth() <= 0 && !m_Won)
	{
		auto sp = m_DeadScreen->GetComponent<SpriteComponent>();
		float a = sp->GetColor().w;
		a = a + gameContext.pGameTime->GetElapsed() * (1 - a);
		sp->SetColor(XMFLOAT4(1,1,1,a));

		if(gameContext.pInput->IsActionTriggered(GameInput::QUIT))
		{
			PostQuitMessage(0);
		}
	}
	else if(!m_Won)
	{
		ManagePlayer(gameContext);	
		SpawnEnemies(gameContext);		
		ManageEnemies(gameContext);
		ManageTurrets(gameContext);
		ManagePickups(gameContext);
	}
}

void MainScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void MainScene::CreateTerrain(const GameContext& gameContext)
{
	auto terrain = new Terrain();
	AddChild(terrain);
	float scale = 0.55f;
	terrain->GetTransform()->Scale(scale,scale,scale);
}

void MainScene::CreateInputs(const GameContext& gameContext)
{
	auto inputAction = InputAction(GameInput::SHOOT, InputTriggerState::Released, VK_SPACE);
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::MOVEUP, InputTriggerState::Down, 'W');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::MOVEDOWN, InputTriggerState::Down, 'S');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::MOVELEFT, InputTriggerState::Down, 'A');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::MOVERIGHT, InputTriggerState::Down, 'D');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::DEBUG, InputTriggerState::Released, VK_TAB);
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(GameInput::QUIT, InputTriggerState::Released, VK_ESCAPE);
	gameContext.pInput->AddInputAction(inputAction);
}

void MainScene::CreateMaterials(const GameContext& gameContext)
{
	// PLAYER
	m_pMatPlayer = new SkinnedDiffuseMaterial();
	//m_pMatPlayer->SetDiffuseTexture(L"./Resources/Textures/furcolor2.png");
	m_pMatPlayer->SetColor(XMFLOAT4(0.15f,0.65f,0.75f,1));
	gameContext.pMaterialManager->AddMaterial(m_pMatPlayer, PlayerMat);
	
	// ENEMY
	auto enemyMat = new SkinnedDiffuseMaterial();
	enemyMat->SetColor(XMFLOAT4(1,0.1f,0.05f,1));
	gameContext.pMaterialManager->AddMaterial(enemyMat, EnemyMat);

	// PROJECTILE
	auto projMat = new ColorMaterial();
	projMat->SetColor(XMFLOAT4(1,0,0,1));
	gameContext.pMaterialManager->AddMaterial(projMat, ProjMat);

	// PICKUP
	auto pickMat = new ColorMaterial();
	pickMat->SetColor(XMFLOAT4(0,0.85f,0.25f,1));
	gameContext.pMaterialManager->AddMaterial(pickMat, PickMat);

	// TERRAIN
	auto terrMat = new DiffuseMaterial();
	terrMat->SetDiffuseTexture(L"./Resources/Textures/terrtex.png");
	//terrMat->SetColor(XMFLOAT4(0.05f,0.1f,0.15f,1));
	gameContext.pMaterialManager->AddMaterial(terrMat, TerrMat);

	// TURRET
		// PLANE
	auto mat1 = new DiffuseMaterial();
	mat1->SetDiffuseTexture(L"./Resources/Textures/glow.png");
	gameContext.pMaterialManager->AddMaterial(mat1, TurrAggro);
	auto mat2 = new DiffuseMaterial();
	mat2->SetDiffuseTexture(L"./Resources/Textures/glowNorm.png");
	gameContext.pMaterialManager->AddMaterial(mat2, TurrNorm);
	auto mat3 = new DiffuseMaterial();
	mat3->SetDiffuseTexture(L"./Resources/Textures/glowOwn.png");
	gameContext.pMaterialManager->AddMaterial(mat3, TurrOwned);
}

LevelPiece* MainScene::CreateLevelPiece(float width, float height, float depth, XMFLOAT3 pos, XMFLOAT4 color)
{
	auto piece = new LevelPiece(width, height, depth, color);
	piece->GetTransform()->Translate(pos.x, height/2 + pos.y, pos.z);
	AddChild(piece);
	return piece;
}

void MainScene::CreateTurrets(const GameContext& gameContext)
{
	// TURRETS
	auto turret = new Turret();
	turret->GetTransform()->Translate(228.5f,15.f,-56.5f);
	AddChild(turret);
	auto t2 = new Turret();
	t2->GetTransform()->Translate(-190,45,37);
	AddChild(t2);
	auto t3 = new Turret();
	t3->GetTransform()->Translate(-330,-15.0f,-325);
	AddChild(t3);
	auto t4 = new Turret();
	t4->GetTransform()->Translate(150, 0, -295);
	AddChild(t4);
	auto t5 = new Turret();
	t5->GetTransform()->Translate(-228.5f,15,370);
	AddChild(t5);
	auto t6 = new Turret();
	t6->GetTransform()->Translate(400,0,370);
	AddChild(t6);
	auto t7 = new Turret();
	t7->GetTransform()->Translate(445,0,-100);
	AddChild(t7);
	auto t8 = new Turret();
	t8->GetTransform()->Translate(-10,0,-55);
	AddChild(t8);

	m_Turrets.push_back(turret);
	m_Turrets.push_back(t2);
	m_Turrets.push_back(t3);
	m_Turrets.push_back(t4);
	m_Turrets.push_back(t5);
	m_Turrets.push_back(t6);
	m_Turrets.push_back(t7);
	m_Turrets.push_back(t8);

	for(int i = 0; i < m_Turrets.size(); ++i)
	{
		auto go = new GameObject();
		auto sp = new SpriteComponent(L"./Resources/Textures/turretRed.png");
		go->AddComponent(sp);
		AddChild(go);
		m_TurretSprites.push_back(go);
	}
}

void MainScene::CreatePickups()
{
	auto pickup = new Pickup();
	pickup->GetTransform()->Translate(-320,0,370);
	AddChild(pickup);
	auto p2 = new Pickup();
	p2->GetTransform()->Translate(250,0,-300);
	AddChild(p2);
	auto p3 = new Pickup("SpeedUp");
	p3->GetTransform()->Translate(152.5f,15,355);
	AddChild(p3);
	auto p4 = new Pickup("SpeedUp");
	p4->GetTransform()->Translate(5,0,-45);
	AddChild(p4);
	auto p5 = new Pickup("SpeedUp");
	p5->GetTransform()->Translate(445,0,50);
	AddChild(p5);
	auto p6 = new Pickup();
	p6->GetTransform()->Translate(-450,0,-450);
	AddChild(p6);

	m_Pickups.push_back(pickup);
	m_Pickups.push_back(p2);
	m_Pickups.push_back(p3);
	m_Pickups.push_back(p4);
	m_Pickups.push_back(p5);
	m_Pickups.push_back(p6);
}

void MainScene::AddProjectile(Projectile* proj)
{
	AddChild(proj);
	m_Projectiles.push_back(proj);
}

void MainScene::SpawnEnemies(const GameContext& gameContext)
{
	SpawnPoint* toDelete = nullptr;

	for(SpawnPoint* spawn : m_SpawnPoints)
	{
		if(spawn->PlayerInRange())
		{
			Enemy* enemy = new Enemy(m_Player);
			AddChild(enemy);
			enemy->GetTransform()->Translate(spawn->GetTransform()->GetWorldPosition());
			m_Enemies.push_back(enemy);
			toDelete = spawn;
		}	
	}

	RemoveFromVec<SpawnPoint>(toDelete, m_SpawnPoints);
}

void MainScene::ManagePlayer(const GameContext& gameContext)
{
	float inv = 8.5f - m_Player->GetInvulTimer();
	if(m_Player->GetInvulTimer() == 0) inv = 0;
	float invPerc = inv / 8.5f;
	XMFLOAT3 blue(0.15f,0.65f,0.75f), white(1,1,1), lerp;
	XMStoreFloat3(&lerp, XMVectorLerp(XMLoadFloat3(&blue), XMLoadFloat3(&white), invPerc));
	m_pMatPlayer->SetColor(XMFLOAT4(lerp.x, lerp.y, lerp.z, 1));

	// BLUR
	float increase(50);
	float dt = gameContext.pGameTime->GetElapsed();
	if(m_Player->BlurScene())
	{
		if(m_MatRemoved)
		{
			m_pBlurMat = new BlurMaterial();
			AddPostProcessingEffect(m_pBlurMat);
			m_MatRemoved = false;
		}

		m_BlurIt += increase * dt;
		m_pBlurMat->SetIterations((int)m_BlurIt);
	}
	else if(m_pBlurMat) 
	{
		m_BlurIt -= increase * dt;
		m_pBlurMat->SetIterations((int)m_BlurIt);
	}

	Clamp<float>(m_BlurIt, 100, 0);
}

void MainScene::ManageHUD(const GameContext& gameContext)
{
	if(gameContext.pInput->IsActionTriggered(GameInput::DEBUG))
	{
		DebugRenderer::ToggleDebugRenderer();
	}

	m_HpBar->SetResource(m_Player->GetHealth(), 150);
	m_SpeedBar->SetResource(m_Player->GetSpeedUp() - 1, 1.75f);
	float inv = 10 - m_Player->GetInvulTimer();
	if(m_Player->GetInvulTimer() == 0) inv = 0;
	m_InvBar->SetResource(inv, 10);
}

void MainScene::ManageTurrets(const GameContext& gameContext)
{
	int i = 0;
	int playerOwnedTurrets = 0;

	for(Turret* turret : m_Turrets)
	{
		if(turret)
		{
			m_TurretSprites[i]->GetTransform()->Translate(10 + i * 64 + i * 10, 10, 0);
			auto sp = m_TurretSprites[i]->GetComponent<SpriteComponent>();
			if(turret->PlayerOwned())
			{
				++playerOwnedTurrets;
				sp->SetTexture(L"./Resources/Textures/turretGreen.png");
			}

			if(turret->CanShoot())
			{
				XMFLOAT3 dest;
				if(!turret->PlayerOwned()) dest = m_Player->GetTransform()->GetPosition();
				else
				{
					Enemy* closest = nullptr;
					float dist = 100000;
					for(Enemy* en : m_Enemies)
					{
						if(en)
						{
							XMFLOAT3 enPos = en->GetTransform()->GetPosition();
							XMFLOAT3 dir = turret->GetTransform()->GetPosition();
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
						dest = closest->GetTransform()->GetPosition();
					}
					else
					{
						turret->SetTargets(0);
						return;
						dest = XMFLOAT3(0,0,0);
					}
				}

				cout << "DEST: " << dest.x << ", " << dest.y << ", " << dest.z << endl;
				dest.y += 1.5f; // small offset

				auto proj = new Projectile(1.25f, 15, dest);
				auto pos = turret->GetTransform()->GetWorldPosition();
				pos.y += 22;
				proj->GetTransform()->Translate(pos);
				AddProjectile(proj);
			}

			++i;
		}
	}

	if(m_Turrets.size() == playerOwnedTurrets)
	{
		m_Won = true;
	}
}

void MainScene::ManageProjectiles(const GameContext& gameContext)
{
	Projectile* toDelete = nullptr;

	for(Projectile* proj : m_Projectiles)
	{
		if(proj)
		{
			if(proj->PlayerHit() || proj->CanDestroy())
			{
				toDelete = proj;		
			}
		}
	}

	RemoveFromVec<Projectile>(toDelete, m_Projectiles);
}

void MainScene::ManageEnemies(const GameContext& gameContext)
{
	Enemy* toDel = nullptr;
	for(Enemy* enemy : m_Enemies)
	{
		if(enemy)
		{
			if(enemy->IsDead()) toDel = enemy;

			enemy->MoveToPlayer(gameContext);
			if(enemy->CanShoot())
			{
				XMFLOAT3 dest = m_Player->GetTransform()->GetPosition();
				XMFLOAT3 pos = enemy->GetTransform()->GetPosition();
				XMFLOAT3 fw = enemy->GetTransform()->GetForward();
				pos.x -= fw.x * 3.5f;
				pos.y += 1;
				pos.z -= fw.z * 3.5f;
				auto proj = new Projectile(0.5f, 10, dest, 100);
				proj->GetTransform()->Translate(pos);
				AddProjectile(proj);
			}
		}
	}

	if(toDel)
	{
		auto fire = new Fire(XMFLOAT3(0,5,0));
		fire->GetTransform()->Translate(toDel->GetTransform()->GetPosition());
		AddChild(fire);
		RemoveFromVec<Enemy>(toDel, m_Enemies);
	}
}

void MainScene::ManagePickups(const GameContext& gameContext)
{
	Pickup* toDelete = nullptr;

	for(Pickup* pickup : m_Pickups)
	{
		if(pickup->IsPickedUp())
		{
			cout << "Pickup up" << endl;
			toDelete = pickup;
		}
	}

	RemoveFromVec<Pickup>(toDelete, m_Pickups);
}

void MainScene::CreateSpawnPoints()
{
	CreateSpawnPoint(0,0,30);
	CreateSpawnPoint(-100,0,50);
	CreateSpawnPoint(120,15,355);
	CreateSpawnPoint(-270,-15,-325);
	CreateSpawnPoint(240,0,-180);
	CreateSpawnPoint(335,0,-100);
	CreateSpawnPoint(-250,45,37);
	CreateSpawnPoint(-400,20,130);
	CreateSpawnPoint(275,0,-300);
	CreateSpawnPoint(200,0,-400);
	CreateSpawnPoint(5,0,-175);
	CreateSpawnPoint(-125,0,-120);
	CreateSpawnPoint(0,0,-225);

	float x1(180), x2(500), z1(-475), z2(-275);
	for(int i = 0; i < 6; ++i)
		CreateSpawnPoint(randF(x1,x2),0,randF(z1,z2));
	x1 = -500; x2 = 500; z1 = -500; z2 = 500;
	for(int i = 0; i < 52; ++i)
		CreateSpawnPoint(randF(x1,x2),0,randF(z1,z2));
	x1 = 440; x2 = 485; z1 = -375; z2 = 400;
	for(int i = 0; i < 6; ++i)
		CreateSpawnPoint(randF(x1,x2),0,randF(z1,z2));
}

void MainScene::CreateSpawnPoint(float x, float y, float z)
{
	auto spawnPoint = new SpawnPoint();
	spawnPoint->GetTransform()->Translate(x,y,z);
	AddChild(spawnPoint);
	m_SpawnPoints.push_back(spawnPoint);
}

template<class T> void MainScene::RemoveFromVec(T* ptr, vector<T*>& vec)
{
	if(ptr)
	{
		for(typename vector<T*>::iterator it = vec.begin(); it != vec.end();)
		{
			if(ptr == *it)
			{
				RemoveChild(*it);
				it = vec.erase(it);
			}
			else
				++it;
		}		
	}
}