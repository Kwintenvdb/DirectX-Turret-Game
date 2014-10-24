#pragma once
#include "BaseComponent.h"

class CameraComponent: public BaseComponent
{
public:
	CameraComponent(void);
	virtual ~CameraComponent(void);

	void SetFieldOfView(float fov) { m_FOV = fov; }
	void SetOrthoSize(float size) { m_Size = size; }
	void SetNearClippingPlane(float nearPlane) { m_NearPlane = nearPlane; }
	void SetFarClippingPlane(float farPlane) { m_FarPlane = farPlane; }

	void UsePerspectiveProjection() { m_PerspectiveProjection = true; }
	void UseOrthographicProjection() { m_PerspectiveProjection = false; }

	void SetActive();
	bool IsActive() const {return m_IsActive;}

	const XMFLOAT4X4& GetView() const {return m_View;}
	const XMFLOAT4X4& GetProjection() const {return m_Projection;}
	const XMFLOAT4X4& GetViewProjection() const {return m_ViewProjection;}
	const XMFLOAT4X4& GetViewInverse() const {return m_ViewInverse;}
	const XMFLOAT4X4& GetViewProjectionInverse() const {return m_ViewProjectionInverse;}

	GameObject* Pick(const GameContext& gameContext) const;
	XMFLOAT3 RaycastPos(const GameContext& gameContext);

protected:
	
	virtual void Update(const GameContext& context);
	virtual void Draw(const GameContext& context);
	virtual void Initialize(const GameContext& gameContext);

private:

	XMFLOAT4X4 m_View, m_Projection, m_ViewInverse, m_ViewProjection, m_ViewProjectionInverse;
	float m_FarPlane, m_NearPlane, m_FOV, m_Size;
	bool m_IsActive, m_PerspectiveProjection;

	friend class GameScene;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	CameraComponent( const CameraComponent &obj);
	CameraComponent& operator=( const CameraComponent& obj);
};

