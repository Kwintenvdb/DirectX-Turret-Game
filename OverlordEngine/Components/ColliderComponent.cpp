//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.82
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "../Base/GeneralStructs.h"
#include "../Diagnostics/Logger.h"
#include "../Scenegraph/GameObject.h"

ColliderComponent::ColliderComponent(std::shared_ptr<PxGeometry> & geometry, const PxMaterial& material, const PxTransform& localPose)
	:
	m_Geometry(geometry),
	m_Material(material),
	m_LocalPose(localPose),
	m_pShape(nullptr),
	m_isTrigger(false)
{

}


ColliderComponent::~ColliderComponent(void)
{
}

void ColliderComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();
		
	if (rigidBody == nullptr)
	{
		Logger::LogError(L"[ColliderComponent] Cannot add a Collider to an object that does not have a rigid body");
		return;
	}

	rigidBody->AddCollider(this);
}

void ColliderComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ColliderComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ColliderComponent::EnableTrigger(bool isTrigger) 
{ 
	m_isTrigger = isTrigger; 
	if(m_pShape != nullptr)
	{
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
	}
}

void ColliderComponent::SetShape(PxShape* shape) 
{ 
	m_pShape = shape; 
	if(m_isTrigger)
	{
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	}
}
