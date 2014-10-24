#pragma once
#include "Scenegraph/GameObject.h"

class Laser : public GameObject
{
public:
	Laser(XMFLOAT3 vel = XMFLOAT3(0,1,0));
	~Laser(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	bool IsDone() { if(m_Timer >= 5) return true; return false; }

private:

	float m_Timer;
	XMFLOAT3 m_Velocity;
};