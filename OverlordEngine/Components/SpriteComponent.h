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

class TextureData;

class SpriteComponent : public BaseComponent
{
public:
	SpriteComponent(const wstring& spriteAsset, XMFLOAT2 pivot = XMFLOAT2(0,0), XMFLOAT4 color = (XMFLOAT4)Colors::White);
	~SpriteComponent(void);

	XMFLOAT2 GetPivot() const		{ return m_Pivot; }
	XMFLOAT4 GetColor() const		{ return m_Color; }
	XMFLOAT2 GetDimension();

	void SetPivot(XMFLOAT2 pivot) { m_Pivot = pivot; }
	void SetColor(XMFLOAT4 color) { m_Color = color; }
	void SetTexture(const wstring& spriteAsset);

protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	TextureData* m_pTexture;
	wstring m_SpriteAsset;
	XMFLOAT2 m_Pivot;
	XMFLOAT4 m_Color;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpriteComponent(const SpriteComponent& obj);
	SpriteComponent& operator=(const SpriteComponent& obj);
};

