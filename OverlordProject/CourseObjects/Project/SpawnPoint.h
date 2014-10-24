#pragma once
#include "Scenegraph/GameObject.h"

class CubePrefab;
class Player;
class MainScene;

class SpawnPoint : public GameObject
{
public:
	SpawnPoint();
	~SpawnPoint(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	bool PlayerInRange() { return m_PlayerInRange; }

private:

	bool m_PlayerInRange;
};