//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.81
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once
#include "../Helpers/EffectHelper.h"

enum MatId : int
{
	TurretMat,
	TurrNorm,
	TurrAggro,
	TurrOwned,
	ProjMat,
	PickMat,
	PlayerMat,
	EnemyMat,
	TerrMat
};

class MeshFilter;
class ModelComponent;
struct GameContext;

class Material
{
public:
	Material(const wstring& effectFile, const wstring& technique = L"");
	virtual ~Material(void);

	void Initialize(const GameContext& gameContext);
	void SetEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);
	ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	ID3D11InputLayout* GetInputLayout(){ return m_pInputLayout; }

	void SetType(wstring type) { m_Type = type; }
	wstring GetType() { return m_Type; }

protected:

	virtual void LoadEffectVariables() = 0;
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) = 0;

	ID3DX11Effect *m_pEffect;
	ID3DX11EffectTechnique *m_pTechnique;

	wstring m_Type;

private:

	friend class MeshFilter;

	bool LoadEffect(const GameContext& gameContext, wstring effectFile);

	ID3DX11EffectMatrixVariable *m_pWorldMatrixVariable;
	ID3DX11EffectMatrixVariable *m_pViewMatrixVariable;
	ID3DX11EffectMatrixVariable *m_pViewInverseMatrixVariable;
	ID3DX11EffectMatrixVariable *m_pWvpMatrixVariable;

	ID3D11InputLayout *m_pInputLayout;
	vector<ILDescription> m_pInputLayoutDescriptions;
	int m_pInputLayoutSize;

	wstring m_effectFile;
	wstring m_TechniqueName;
	bool m_IsInitialized;
	UINT m_InputLayoutID;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Material(const Material& yRef);
	Material& operator=(const Material& yRef);
};

