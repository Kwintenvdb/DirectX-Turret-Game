//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.113
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once
#include "BaseComponent.h"

class MeshFilter;
class ModelAnimator;
class Material;

class ModelComponent: public BaseComponent
{
public:
	ModelComponent(wstring assetFile);
	~ModelComponent();

	void SetMaterial(UINT materialId);
	void SetMaterial(Material* pMat);
	ModelAnimator* GetAnimator() const { return m_pAnimator; }
	bool HasAnimator() const { return m_pAnimator != 0; }

protected:

	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);
	virtual void PostDraw(const GameContext& gameContext);
	virtual void Initialize(const GameContext& gameContext);

private:

	void UpdateMaterial(const GameContext& gameContext);

	wstring m_AssetFile;
	MeshFilter *m_pMeshFilter;
	ModelAnimator* m_pAnimator;
	Material *m_pMaterial;
	UINT m_MaterialId;
	bool m_MaterialSet, m_IgnoreId;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ModelComponent(const ModelComponent& yRef);
	ModelComponent& operator=(const ModelComponent& yRef);

};

