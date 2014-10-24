#pragma once
#include "Scenegraph/GameObject.h"

class CubePrefab;
class Player;
class MainScene;
class ModelComponent;
class ColorMaterial;
class Enemy;

class Turret : public GameObject
{
public:
	Turret();
	~Turret(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	XMVECTOR GetDir()			{ return m_NormDir; }
	bool CanShoot()				{ return m_CanShoot; }
	bool PlayerOwned()			{ return m_PlayerOwned; }
	GameObject* GetTarget()			{ return m_Target; }
	vector<Enemy*> GetEnemies() { return m_Enemies; }

	void SetTargets(int t)		{ m_Targets = t; }

private:

	void ShootPlayer();
	void ManageOwnership();
	void FindClosestEnemy();

	CubePrefab* m_pCube;
	XMVECTOR m_NormDir;
	float m_Timer;
	float m_DeltaTime;

	bool m_PlayerInRange;
	bool m_CanShoot;
	int m_Damage;
	float m_PlayerTimer;
	bool m_StartPlayerTimer;
	bool m_PlayerOwned;

	GameObject* m_HP;
	ModelComponent* m_pMC;
	ColorMaterial* m_pColMat;

	vector<Enemy*> m_Enemies;
	GameObject* m_Target;
	int m_Targets;
};