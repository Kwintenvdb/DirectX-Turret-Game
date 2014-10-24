#pragma once
#include "Scenegraph/GameObject.h"

class Fire : public GameObject
{
public:
	Fire(XMFLOAT3 vel = XMFLOAT3(0,1,0));
	~Fire(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

	bool IsLit() { return m_IsLit; }
	bool IsDone() { if(m_Timer >= 5) return true; return false; }

private:

	bool m_IsLit;
	float m_Timer;
	XMFLOAT3 m_Velocity;
};