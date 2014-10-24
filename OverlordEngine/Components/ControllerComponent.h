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
#pragma once
#include "BaseComponent.h"

class ControllerComponent : public BaseComponent
{
public:
	ControllerComponent(PxMaterial* material, float radius = 2, float height = 5, wstring name = L"Character", PxCapsuleClimbingMode::Enum climbingMode = PxCapsuleClimbingMode::eEASY);
	virtual ~ControllerComponent();

	void Translate(XMFLOAT3 position);
	void Translate(float x, float y, float z);
	void Move(XMFLOAT3 displacement, float minDist = 0);
	const PxControllerCollisionFlags& GetCollisionFlags() const { return m_CollisionFlag; }

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetFootPosition();

	void SetCollisionGroup(CollisionGroupFlag group); 
	void SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups);

protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	float m_Radius, m_Height;
	wstring m_Name;
	PxController* m_ControllerPtr;
	PxCapsuleClimbingMode::Enum m_ClimbingMode;
	PxMaterial* m_pMaterial;
	PxControllerCollisionFlags m_CollisionFlag;

	PxFilterData m_CollisionGroups;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ControllerComponent( const ControllerComponent& obj);
	ControllerComponent& operator=( const ControllerComponent& obj);
};

