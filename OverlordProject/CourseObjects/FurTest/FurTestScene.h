#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"
#include "Graphics\SpriteFont.h"

class GameObject;
class FurMaterial;
class SpriteFont;

class FurTestScene: public GameScene
{
public:
	FurTestScene(void);
	virtual ~FurTestScene(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	void CreateInputs(const GameContext& gameContext);

	GameObject* m_pFurObj;
	FurMaterial* m_pFur;
	SpriteFont* m_pFont;

	float m_MaxHair;
	float m_Layers;
	XMFLOAT3 m_Comb;
	bool m_SetSway;
	float m_Rotation;
	bool m_bRotate;

	vector<wstring> m_FurTextures;
	int m_CurrentFur;
	wstringstream m_OldFPS, m_OldMil;
	float m_UpdateText;
	bool m_ShowText;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	FurTestScene( const FurTestScene &obj);
	FurTestScene& operator=( const FurTestScene& obj);
};