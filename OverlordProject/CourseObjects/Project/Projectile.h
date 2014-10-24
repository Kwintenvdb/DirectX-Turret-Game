#pragma once
#include "Scenegraph/GameObject.h"

class ColliderComponent;
class SpherePrefab;
class RigidBodyComponent;

class Projectile : public GameObject
{
public:
	Projectile(float size, int damage, XMFLOAT3 destination, float speed = 65, XMFLOAT4 color = (XMFLOAT4)Colors::Red);
	~Projectile(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void PostInitialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	void Shoot(const PxVec3& force, const GameContext& gameContext);

	bool PlayerHit() { return m_PlayerHit; }
	bool CanDestroy();

private:

	float m_Size;
	float m_Speed;
	int m_Damage;
	XMFLOAT3 m_Destination, m_Direction;
	XMFLOAT4 m_Color;
	SpherePrefab *m_pSphere;
	RigidBodyComponent *m_RigidBody;
	PxVec3 m_ForceDirection;

	bool m_PlayerHit, m_CanDestroy;
	float m_Timer;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Projectile(const Projectile& t);
	Projectile& operator=(const Projectile& t);
};