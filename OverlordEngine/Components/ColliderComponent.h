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

class ColliderComponent : public BaseComponent
{
public:
	ColliderComponent(std::shared_ptr<PxGeometry>& geometry, const PxMaterial& material, const PxTransform& localPose = PxTransform::createIdentity());
	~ColliderComponent(void);

	PxShape* GetShape() {return m_pShape; };
	bool IsTrigger(){return m_isTrigger;};
	void EnableTrigger(bool isTrigger);

	

protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	void SetShape(PxShape* shape);

	std::shared_ptr<PxGeometry> m_Geometry;
	const PxMaterial& m_Material;
	const PxTransform m_LocalPose;

	PxShape* m_pShape;
	bool m_isTrigger;

	friend class RigidBodyComponent;
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ColliderComponent( const ColliderComponent& obj);
	ColliderComponent& operator=( const ColliderComponent& obj);
};

