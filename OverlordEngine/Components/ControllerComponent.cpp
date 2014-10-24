//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.81
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "../Base/OverlordGame.h"
#include "../Physx/PhysxProxy.h"
#include "../Physx/PhysxManager.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"
#include "../Helpers/PhysxHelper.h"
#include "../Diagnostics/Logger.h"

ControllerComponent::ControllerComponent(PxMaterial* material, float radius, float height, wstring name, PxCapsuleClimbingMode::Enum climbingMode)
	: m_pMaterial(material),
	m_Radius(radius),
	m_Height(height),
	m_Name(name),
	m_ClimbingMode(climbingMode),
	m_ControllerPtr(nullptr),
	m_CollisionFlag(PxControllerCollisionFlags()),
	m_CollisionGroups(PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0))
{
}


ControllerComponent::~ControllerComponent()
{
	if(m_ControllerPtr != nullptr) m_ControllerPtr->release();
}

void ControllerComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if(m_ControllerPtr != nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot initialize a controller twice");

	//1. Create a PxCapsuleControllerDesc (fill in all the required fields)
	PxCapsuleControllerDesc controllerDesc;
	controllerDesc.setToDefault();
	controllerDesc.material = m_pMaterial;
	controllerDesc.radius = m_Radius;
	controllerDesc.height = m_Height;
	controllerDesc.climbingMode = m_ClimbingMode;
	controllerDesc.userData = this;
	controllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;

	//2. Retrieve the controller manager from the PhysXProxy (GameScene>PhysxProxy)

	
	PhysxProxy* physxProxyPtr = m_pGameObject->GetScene()->GetPhysxProxy();
	m_ControllerPtr = physxProxyPtr->GetControllerManager()->createController(controllerDesc);
	

	//		Use the manager to create a PxController

	if(m_ControllerPtr == nullptr)
		Logger::LogError(L"[ControllerComponent] Failed to create controller");

	//3. Retrieve the controller actor and give it a name (see PxRigidDynamic documentation)
	//			Use m_Name variable as name (wstring -> string)

	string name(m_Name.begin(), m_Name.end());
	m_ControllerPtr->getActor()->setName(name.c_str());

	m_ControllerPtr->getActor()->userData = this; 
	SetCollisionGroup((CollisionGroupFlag) m_CollisionGroups.word0); 
	SetCollisionIgnoreGroups((CollisionGroupFlag) m_CollisionGroups.word1);
}

void ControllerComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ControllerComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
void ControllerComponent::Translate(XMFLOAT3 position)
{
	if(m_ControllerPtr == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Translate. No controller present");
	else
		m_ControllerPtr->setPosition(ToPxExtendedVec3(position));
}

void ControllerComponent::Translate(float x, float y, float z)
{
	Translate(XMFLOAT3(x,y,z));
}

void ControllerComponent::Move(XMFLOAT3 displacement, float minDist)
{
	if(m_ControllerPtr == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Move. No controller present");
	else
		m_CollisionFlag = m_ControllerPtr->move(ToPxVec3(displacement), minDist, 0, 0, 0);
}

XMFLOAT3 ControllerComponent::GetPosition()
{
	if(m_ControllerPtr == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get position. No controller present");
	else
		return ToXMFLOAT3(m_ControllerPtr->getPosition());

	return XMFLOAT3();
}

XMFLOAT3 ControllerComponent::GetFootPosition()
{
	if(m_ControllerPtr == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get footposition. No controller present");
	else
		return ToXMFLOAT3(m_ControllerPtr->getFootPosition());

	return XMFLOAT3();
}

void ControllerComponent::SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups)
{ 
	m_CollisionGroups.word1 = ignoreGroups;

	if (!m_ControllerPtr)
		return;

	auto nbShapes = m_ControllerPtr->getActor()->getNbShapes();
	PxShape* buffer; m_ControllerPtr->getActor()->getShapes(&buffer, nbShapes * sizeof(PxShape));

	for (int i = 0; i < nbShapes; ++i) 
	{ 
		buffer[i].setSimulationFilterData(m_CollisionGroups); 
	} 
}

void ControllerComponent::SetCollisionGroup(CollisionGroupFlag group)
{ 
	m_CollisionGroups.word0 = group;

	if (!m_ControllerPtr) 
		return;

	auto nbShapes = m_ControllerPtr->getActor()->getNbShapes();
	PxShape* buffer; m_ControllerPtr->getActor()->getShapes(&buffer, nbShapes * sizeof(PxShape));

	for (int i = 0; i < nbShapes; ++i) 
	{ 
		buffer[i].setSimulationFilterData(m_CollisionGroups); 
	} 
}