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
#pragma once
#include "..\Scenegraph\GameObject.h"

class TorusPrefab: public GameObject
{
public:
	TorusPrefab(float majorRadius = 1.f, UINT majorRadiusSteps = 10, float minorRadius = .2f, UINT minorRadiusSteps = 5, XMFLOAT4 color = (XMFLOAT4)Colors::Green);
	~TorusPrefab(void);

protected:

	virtual void Initialize(const GameContext& gameContext);

private:

	float m_MajorRadius, m_MinorRadius;
	UINT m_MajorRadiusSteps, m_MinorRadiusSteps;
	XMFLOAT4 m_Color;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TorusPrefab(const TorusPrefab& yRef);									
	TorusPrefab& operator=(const TorusPrefab& yRef);
};
