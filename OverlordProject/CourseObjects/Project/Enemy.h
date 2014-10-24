#pragma once
#include "Scenegraph/GameObject.h"

class CubePrefab;
class Player;
class ControllerComponent;
class ModelComponent;

class Enemy : public GameObject
{
public:
	Enemy(Player* player);
	~Enemy(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	void MoveToPlayer(const GameContext& gameContext);
	void Damage(int hp)		{ 
								m_Health -= hp; 
								m_AddFire = true;
							}
	void PushBack();
	bool CanShoot()			{ return m_CanShoot; }
	bool IsDead()			{ if(m_Health <= 0) return true; return false; }

private:

	void ToggleMovement()	{ m_CanMove = !m_CanMove; }

	Player* m_pPlayer;
	ControllerComponent* m_Controller;
	ModelComponent* m_pModelComp;
	CubePrefab* m_pCube;
	bool m_CanMove, m_CanPushBack, m_CanShoot, m_AddFire;
	float m_Timer, m_ShootTimer;
	float m_FallVelocity;
	float m_Angle;
	float m_DistanceToPlayer;
	int m_Health;
};