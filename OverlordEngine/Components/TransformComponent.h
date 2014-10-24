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

class TransformComponent: public BaseComponent
{
public:
	TransformComponent(void);
	virtual ~TransformComponent(void);

	void Translate(float x, float y, float z);
	void Translate(const XMFLOAT3& position);
	void Translate(const XMVECTOR& position);

	void Rotate(float x, float y, float z, bool isEuler = true);
	void Rotate(const XMFLOAT3& rotation, bool isEuler = true);
	void Rotate(const XMVECTOR& rotation, bool isQuaternion = true);

	void Scale(float x, float y, float z);
	void Scale(const XMFLOAT3& scale);

	const XMFLOAT3& GetPosition() const { return m_Position; }
	const XMFLOAT3& GetWorldPosition() const { return m_WorldPosition; }
	const XMFLOAT3& GetScale() const { return m_Scale; }
	const XMFLOAT3& GetWorldScale() const { return m_WorldScale; }
	const XMFLOAT4& GetRotation() const { return m_Rotation; }
	//const XMFLOAT3& GetRotationEuler() const;
	const XMFLOAT4& GetWorldRotation() const { return m_WorldRotation; }
	const XMFLOAT4X4& GetWorld() const { return m_World; }

	const XMFLOAT3& GetForward() const { return m_Forward; }
	const XMFLOAT3& GetUp() const { return m_Up; }
	const XMFLOAT3& GetRight() const { return m_Right; }

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);


	void UpdateTransforms(const GameContext& gameContext);
	bool CheckConstraints();

private:
	enum TransformChanged{
		NONE = 0x00,
		TRANSLATION = 0x01,
		ROTATION = 0x02,
		SCALE = 0x04,
	};

	unsigned char m_IsTransformChanged;

	XMFLOAT3 m_Position, m_WorldPosition, m_Scale, m_WorldScale, m_Forward, m_Up, m_Right;
	XMFLOAT4 m_Rotation, m_WorldRotation;
	XMFLOAT4X4 m_World;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TransformComponent( const TransformComponent& obj);
	TransformComponent& operator=( const TransformComponent& obj);
};
