#include "Base\stdafx.h"

#include "Components\Components.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "LevelPiece.h"
#include "Player.h"
#include "Projectile.h"
#include "MainScene.h"
#include "../../Materials/ColorMaterial.h"
#include "Scenegraph/GameScene.h"

LevelPiece::LevelPiece(float width, float height, float depth, XMFLOAT4 color) :
	m_pCube(nullptr),
	m_Width(width),
	m_Height(height),
	m_Depth(depth),
	m_Color(color)
{
	SetName("LevelPiece");
}

LevelPiece::~LevelPiece(void)
{

}

void LevelPiece::Initialize(const GameContext& gameContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto defaultMaterial = physX->createMaterial(0, 0, 1);

	m_pCube = new CubePrefab(m_Width,m_Height,m_Depth,m_Color);
	AddChild(m_pCube);
	
	shared_ptr<PxGeometry> geom(new PxBoxGeometry(m_Width/2,m_Height/2,m_Depth/2));
	auto col = new ColliderComponent(geom, *defaultMaterial);
	AddComponent(new RigidBodyComponent(true));	
	AddComponent(col);
}

void LevelPiece::Update(const GameContext& gameContext)
{

}
