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

#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "../Base/GeneralStructs.h"
#include "../Base/stdafx.h"
#include "../Diagnostics/Logger.h"
#include "../Helpers/PhysxHelper.h"
#include "../Physx/PhysxProxy.h"
#include "../Physx/PhysxManager.h"
#include "../SceneGraph/GameObject.h"
#include "../SceneGraph/GameScene.h"

RigidBodyComponent::RigidBodyComponent(bool isStatic) :
m_pActor(nullptr),
m_isStatic(isStatic),
m_isKinematic(false),
m_pConstraintJoint(nullptr),
m_InitialConstraints(0),
m_CollisionGroups(PxFilterData(CollisionGroupFlag::Group0,0,0,0))
{
}


RigidBodyComponent::~RigidBodyComponent(void)
{
	if(m_pActor != nullptr)
	{
		auto physxScene = GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene();
		physxScene->removeActor(*m_pActor);
		m_pActor->release();
	}
}

void RigidBodyComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void RigidBodyComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void RigidBodyComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void RigidBodyComponent::AddCollider(ColliderComponent* collider) 
{ 

	m_Colliders.push_back(collider); 
	if(m_pActor == nullptr)
		CreateActor();
	else
		collider->SetShape(m_pActor->createShape(*collider->m_Geometry, collider->m_Material, collider->m_LocalPose));

	collider->GetShape()->setSimulationFilterData(m_CollisionGroups);
}

void RigidBodyComponent::SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups)
{
	m_CollisionGroups.word1 = ignoreGroups;
	for each (auto shape in m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
	}
}

void RigidBodyComponent::SetCollisionGroup(CollisionGroupFlag group)
{
	m_CollisionGroups.word0 = group;
	for each (auto shape in m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
	}
}

void RigidBodyComponent::SetKinematic(bool isKinematic)
{
	if (m_isStatic && isKinematic)
		Logger::LogError(L"[RigidBodyComponent] A static RigidBody can never be kinematic");

	m_isKinematic = isKinematic;

	if(m_pActor != nullptr && !m_isStatic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, m_isKinematic);
}

void RigidBodyComponent::CreateActor()
{
	if(m_pActor != nullptr)
		Logger::LogError(L"[RigidBodyComponent] CreateActor cannot be called twice");

	if (m_Colliders.size() == 0)
		return;

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto physxScene = GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene();

	auto transform = GetTransform();

	if(m_isStatic)
		m_pActor = physX->createRigidStatic(PxTransform(ToPxVec3(transform->GetPosition())));
	else
		m_pActor = physX->createRigidDynamic(PxTransform(ToPxVec3(transform->GetPosition())));

	if (!m_isStatic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, m_isKinematic);
		m_pConstraintJoint = PxD6JointCreate(*physX, nullptr, PxTransform(PxVec3(0, 0, 0)), m_pActor, PxTransform(PxVec3(0, 0, 0)));

		SetConstraint(RigidBodyConstraintFlag::RotX, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::RotX) == RigidBodyConstraintFlag::RotX));
		SetConstraint(RigidBodyConstraintFlag::RotY, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::RotY) == RigidBodyConstraintFlag::RotY));
		SetConstraint(RigidBodyConstraintFlag::RotZ, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::RotZ) == RigidBodyConstraintFlag::RotZ));
		SetConstraint(RigidBodyConstraintFlag::TransX, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::TransX) == RigidBodyConstraintFlag::TransX));
		SetConstraint(RigidBodyConstraintFlag::TransY, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::TransY) == RigidBodyConstraintFlag::TransY));
		SetConstraint(RigidBodyConstraintFlag::TransZ, (bool) ((m_InitialConstraints&RigidBodyConstraintFlag::TransZ) == RigidBodyConstraintFlag::TransZ));
	}

	for (auto collider : m_Colliders)
		collider->SetShape(m_pActor->createShape(*collider->m_Geometry, collider->m_Material, collider->m_LocalPose));

	physxScene->addActor(*m_pActor);
	m_pActor->userData = this;

	

}

void RigidBodyComponent::Translate(XMFLOAT3 position)
{
	if(m_pActor != nullptr && !m_isStatic)
	{
		if(!m_isKinematic)
			m_pActor->setGlobalPose(PxTransform(ToPxVec3(position), ToPxQuat(GetRotation())));
		else
			reinterpret_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(PxTransform(ToPxVec3(position), ToPxQuat(GetRotation())));
	}
}

void RigidBodyComponent::Rotate(XMFLOAT4 rotation)
{
	if(m_pActor != nullptr && !m_isStatic)
	{
		if(!m_isKinematic)
			m_pActor->setGlobalPose(PxTransform(ToPxVec3(GetPosition()), ToPxQuat(rotation)));
		else
			reinterpret_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(PxTransform(ToPxVec3(GetPosition()), ToPxQuat(rotation)));
	}
}

XMFLOAT3 RigidBodyComponent::GetPosition()
{
	if(m_pActor != nullptr)
	{
		PxTransform pose;
		if(!m_isKinematic || !reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(pose))
			pose = m_pActor->getGlobalPose();

		return XMFLOAT3(pose.p.x, pose.p.y, pose.p.z);
	}
	Logger::LogError(L"[RigidBodyComponent] Calling GetPosition on a rigid body without actor. Did you forget to add a collider?");
	return XMFLOAT3();
}

XMFLOAT4 RigidBodyComponent::GetRotation()
{
	if(m_pActor != nullptr)
	{
		PxTransform pose;
		if(!m_isKinematic || !reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(pose))
			pose = m_pActor->getGlobalPose();
		
		return XMFLOAT4(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
	}
	Logger::LogError(L"[RigidBodyComponent] Calling GetRotation on a rigid body without actor");
	return XMFLOAT4();
}

PxRigidBody* RigidBodyComponent::GetPxRigidBody()
{
		return reinterpret_cast<PxRigidBody*>(m_pActor);
}

void RigidBodyComponent::PutToSleep()
{
	if(m_pActor != nullptr && !m_isStatic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->putToSleep();
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot put a static actor to sleep");
}

void RigidBodyComponent::AddForce(const PxVec3& force, PxForceMode::Enum mode, bool autowake)
{
	if(m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->addForce(force, mode, autowake);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot add a force to a static or kinematic actor");
}

void RigidBodyComponent::AddTorque(const PxVec3& torque, PxForceMode::Enum mode, bool autowake)
{
	if(m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->addTorque(torque, mode, autowake);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot add a torque to a static or kinematic actor");
}

void RigidBodyComponent::ClearForce(PxForceMode::Enum mode)
{
	if(m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->clearForce(mode);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot clear a force on a static or kinematic actor");
}

void RigidBodyComponent::ClearTorque(PxForceMode::Enum mode)
{
	if(m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->clearTorque(mode);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot clear a torque on a static or kinematic actor");
}

void RigidBodyComponent::SetConstraint(RigidBodyConstraintFlag flag, bool enable)
{
	if (m_isStatic)
	{
		Logger::LogWarning(L"RigidBodyComponent::SetConstraint() > Can't contrain a static actor!");
		return;
	}

	if (m_pConstraintJoint)
	{
		switch (flag)
		{
			case RotX:
				m_pConstraintJoint->setMotion(PxD6Axis::eTWIST, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			case RotY:
				m_pConstraintJoint->setMotion(PxD6Axis::eSWING1, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			case RotZ:
				m_pConstraintJoint->setMotion(PxD6Axis::eSWING2, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			case TransX:
				m_pConstraintJoint->setMotion(PxD6Axis::eX, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			case TransY:
				m_pConstraintJoint->setMotion(PxD6Axis::eY, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			case TransZ:
				m_pConstraintJoint->setMotion(PxD6Axis::eZ, enable ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
				break;
			default:
				break;
		}
	}
	else
	{
		m_InitialConstraints |= enable ? flag : ~flag;
	}
}
