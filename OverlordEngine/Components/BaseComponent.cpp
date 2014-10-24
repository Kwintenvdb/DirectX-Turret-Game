//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.121
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "BaseComponent.h"
#include "../Scenegraph/GameObject.h"
#include "../Diagnostics/Logger.h"

BaseComponent::BaseComponent(void):
	m_IsInitialized(false),
	m_pGameObject(nullptr)
{
}


BaseComponent::~BaseComponent(void)
{
}

void BaseComponent::RootInitialize(const GameContext& gameContext)
{
	if(m_IsInitialized)
		return;

	Initialize(gameContext);

	m_IsInitialized = true;
}

void BaseComponent::PostDraw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

TransformComponent* BaseComponent::GetTransform() const
{
#if _DEBUG
	if(!m_pGameObject)
	{
		Logger::LogWarning(L"BaseComponent::GetTransform() > Failed to retrieve the TransformComponent. GameObject is NULL.");
		return nullptr;
	}
#endif

	return m_pGameObject->GetTransform();
}
