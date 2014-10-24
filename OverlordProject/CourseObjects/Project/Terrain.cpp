#include "Base\stdafx.h"
#include "Terrain.h"
#include "Components\Components.h"
#include "Physx/PhysxManager.h"
#include "Prefabs\Prefabs.h"
#include "Content/ContentManager.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"

Terrain::Terrain() 
{
	SetName("Terrain");
}

Terrain::~Terrain(void)
{

}

void Terrain::Initialize(const GameContext& gameContext)
{
	// TERRAIN
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);

	auto terrMat = physX->createMaterial(0.75f, 0.75f, 0.1f);
	auto terrPxMesh = ContentManager::Load<PxTriangleMesh>(L"./Resources/Meshes/terrain2.ovpt");
	shared_ptr<PxGeometry> terrGeom(new PxTriangleMeshGeometry(terrPxMesh));
	auto terrColl = new ColliderComponent(terrGeom, *terrMat);
	AddComponent(terrColl);

	auto terrModel = new ModelComponent(L"./Resources/Meshes/terrain2.ovm");
	terrModel->SetMaterial(TerrMat);
	AddComponent(terrModel);
}

void Terrain::Update(const GameContext& gameContext)
{

}