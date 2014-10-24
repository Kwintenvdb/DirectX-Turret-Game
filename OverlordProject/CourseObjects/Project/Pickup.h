#pragma once
#include "Scenegraph/GameObject.h"

class CubePrefab;
class Player;

class Pickup : public GameObject
{
public:
	Pickup(string name = "");
	~Pickup(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	virtual void OnPickup(Player* player);

	bool IsPickedUp() { return m_IsPickedUp; }

private:

	CubePrefab* m_pCube;
	float m_Timer;
	float m_DeltaTime;
	bool m_IsPickedUp;
	XMFLOAT3 m_Rotation;
};