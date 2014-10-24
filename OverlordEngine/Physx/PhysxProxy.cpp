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

#include "PhysxProxy.h"
#include "../Base/GeneralStructs.h"
#include "../Components/Components.h"
#include "../Diagnostics/Logger.h"
#include "../Diagnostics/DebugRenderer.h"
#include "../Helpers/PhysxHelper.h"
#include "../Scenegraph/GameScene.h"
#include "../Scenegraph/GameObject.h"
#include "PhysxManager.h"

#include "characterkinematic/PxControllerManager.h"

bool PhysxProxy::m_PhysXFrameStepping = false;
float PhysxProxy::m_PhysXStepTime = 0.f;

PhysxProxy::PhysxProxy(void) :
	m_pPhysxScene(nullptr),
	m_DrawPhysx(false),
	m_pControllerManager(nullptr)
{
}


PhysxProxy::~PhysxProxy(void)
{
	if(m_pControllerManager != nullptr)
		m_pControllerManager->release();
	if(m_pPhysxScene != nullptr)
		m_pPhysxScene->release();
}

void PhysxProxy::Initialize(GameScene* parent)
{
	if(m_pPhysxScene != nullptr)
	{
		Logger::LogError(L"[PhysicsProxy] Initialize called twice!");
		return;
	}

	m_pPhysxScene = PhysxManager::GetInstance()->CreateScene(parent);
	if(m_pPhysxScene == nullptr)
	{
		Logger::LogError(L"[PhysicsProxy] Failed to create physx scene!");
		return;
	}

	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	m_pPhysxScene->setSimulationEventCallback(this);

	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	if(m_pControllerManager == nullptr)
		Logger::LogError(L"[PhysicsProxy] Failed to create controller manager!");
}

void PhysxProxy::Update(const GameContext& gameContext)
{

	if(gameContext.pGameTime->IsRunning() && gameContext.pGameTime->GetElapsed() > 0)
	{
		if(m_PhysXFrameStepping)
		{
			if(m_PhysXStepTime > 0.f)
			{
				m_pPhysxScene->simulate(m_PhysXStepTime);
				m_pPhysxScene->fetchResults(true);
				m_PhysXStepTime = 0.f;
			}
			else if(m_PhysXStepTime < 0.f)
			{
				m_pPhysxScene->simulate(gameContext.pGameTime->GetElapsed());
				m_pPhysxScene->fetchResults(true);
			}
		}
		else
		{
			m_pPhysxScene->simulate(gameContext.pGameTime->GetElapsed());
			m_pPhysxScene->fetchResults(true);
		}
	}

	//Send Camera to PVD
	if(m_pPhysxScene->getPhysics().getPvdConnectionManager())
	{
		auto cam = gameContext.pCamera;
		auto transform = cam->GetTransform();
		m_pPhysxScene->getPhysics().getPvdConnectionManager()->setCamera("SceneCam", ToPxVec3(transform->GetPosition()), ToPxVec3(transform->GetUp()), ToPxVec3(transform->GetForward()));
	}
}

void PhysxProxy::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if(m_DrawPhysx)
		DebugRenderer::DrawPhysX(m_pPhysxScene);
}

void PhysxProxy::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	UNREFERENCED_PARAMETER(constraints);
	UNREFERENCED_PARAMETER(count);
}

void PhysxProxy::onWake(PxActor **actors, PxU32 count) 
{
	UNREFERENCED_PARAMETER(actors);
	UNREFERENCED_PARAMETER(count);
}

void PhysxProxy::onSleep(PxActor **actors, PxU32 count) 
{
	UNREFERENCED_PARAMETER(actors);
	UNREFERENCED_PARAMETER(count);
}

void PhysxProxy::onContact(const PxContactPairHeader & pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
{
	UNREFERENCED_PARAMETER(pairHeader);
	UNREFERENCED_PARAMETER(pairs);
	UNREFERENCED_PARAMETER(nbPairs);
}

void PhysxProxy::onTrigger(PxTriggerPair *pairs, PxU32 count) 
{
	UNREFERENCED_PARAMETER(pairs);
	UNREFERENCED_PARAMETER(count);

	for(PxU32 i=0; i < count; i++)
    {
        // ignore pairs when shapes have been deleted
        if (pairs[i].flags & (PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER | PxTriggerPairFlag::eDELETED_SHAPE_OTHER))
            continue;

		auto triggerComponent = reinterpret_cast<BaseComponent*>(((pairs[i].triggerShape)->getActor())->userData);
		auto otherComponent = reinterpret_cast<BaseComponent*>(((pairs[i].otherShape)->getActor())->userData);


		if (triggerComponent != nullptr && otherComponent != nullptr)
		{
			GameObject* trigger = triggerComponent->GetGameObject();
			GameObject* other = otherComponent->GetGameObject();

			if(pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				trigger->OnTrigger(trigger, other,GameObject::ENTER);
			else if(pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				trigger->OnTrigger(trigger, other,GameObject::LEAVE);
		}
    }
}

bool PhysxProxy::Raycast(const PxVec3& origin, const PxVec3& unitDir, const PxReal distance, PxRaycastCallback& hitCall, PxHitFlags hitFlags,
									const PxQueryFilterData& filterData, PxQueryFilterCallback* filterCall, const PxQueryCache* cache)
{
	if(m_pPhysxScene != nullptr)
	{
		return m_pPhysxScene->raycast(origin, unitDir, distance, hitCall, hitFlags,  filterData, filterCall, cache);
	}
	Logger::LogError(L"[PhysicsProxy] Raycast failed. Physics scene is a null pointer");
		return false;
}
