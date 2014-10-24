#pragma once
#include "Scenegraph/GameObject.h"

class ControllerComponent;
class ModelComponent;
class Fire;

class Player : public GameObject
{
public:
	Player();
	~Player(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);
	void Raycast(const GameContext& gameContext);

	void Damage(float amount);
	float GetHealth()			{ return m_Health; }
	void SetInvulnerable(bool invulnerable) { m_Invulnerable = invulnerable; }
	bool IsInvulnerable()		{ return m_Invulnerable; }
	float GetInvulTimer()		{ return m_InvulTimer; }
	void SpeedUp(float amount)	{ m_SpeedUp = amount; }
	float GetSpeedUp()			{ return m_SpeedUp; }
	bool BlurScene()			{ if(m_DamageTimer > 0) return true; else return false; }

private:

	GameObject* m_CamObj, *m_HP;
	ControllerComponent* m_pController;
	ModelComponent* m_pModelComp;
	XMFLOAT3 m_Velocity;
	float m_Gravity,
		  m_JumpAcceleration,
		  m_JumpVelocity,
		  m_RunVelocity,
		  m_RotationSpeed,
		  m_TotalYaw, 
		  m_TotalPitch;

	float m_Health;
	bool  m_Invulnerable;
	float m_InvulTimer;
	float m_SpeedUp;
	float m_DamageTimer;
	float m_LastDamaged;
	float m_CamRot;
	bool  m_HeadAnim;
	float m_ShootTimer;

	bool m_AddFire;
	vector<Fire*> m_Fires;
};