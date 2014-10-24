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
#include "BaseComponent.h"

class ColliderComponent;

enum RigidBodyConstraintFlag
{
	RotX = (1 << 0),
	RotY = (1 << 1),
	RotZ = (1 << 2),
	TransX = (1 << 3),
	TransY = (1 << 4),
	TransZ = (1 << 5)
};

class RigidBodyComponent : public BaseComponent
{
public:
	RigidBodyComponent(bool isStatic = false);
	virtual ~RigidBodyComponent(void);

	virtual bool IsStatic() { return m_isStatic; };
	virtual bool IsDynamic() { return !m_isStatic; };
	virtual bool IsKinematic() { return m_isKinematic; };

	void SetKinematic(bool isKinematic); 
	void PutToSleep();

	PxRigidBody* GetPxRigidBody();
	void AddForce(const PxVec3& force, PxForceMode::Enum mode = PxForceMode::eFORCE, bool autowake = true);
	void AddTorque(const PxVec3& torque, PxForceMode::Enum mode = PxForceMode::eFORCE, bool autowake = true);
	void ClearForce(PxForceMode::Enum mode = PxForceMode::eFORCE);
	void ClearTorque(PxForceMode::Enum mode = PxForceMode::eFORCE);
	void SetConstraint(RigidBodyConstraintFlag flag, bool enable);
	void SetCollisionGroup(CollisionGroupFlag group);
	void SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

	virtual void CreateActor();
	void AddCollider(ColliderComponent* collider); 

	void Translate(XMFLOAT3 position);
	void Rotate(XMFLOAT4 rotation);

	XMFLOAT3 GetPosition();
	XMFLOAT4 GetRotation();

	physx::PxRigidActor* m_pActor;
	vector<ColliderComponent*> m_Colliders;
	
	bool m_isStatic;
	bool m_isKinematic;

	PxD6Joint* m_pConstraintJoint;
	PxFilterData m_CollisionGroups;

private:

	UINT m_InitialConstraints;

private:

	friend class TransformComponent;
	friend class ColliderComponent;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	RigidBodyComponent( const RigidBodyComponent& obj);
	RigidBodyComponent& operator=( const RigidBodyComponent& obj);
};
