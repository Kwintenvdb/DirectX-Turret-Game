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
#include "..\..\OverLordEngine\Graphics\Material.h"

class TextureData;

class FurMaterial : public Material
{
public:
	FurMaterial(float maxHairLength, float nrOfLayers);
	~FurMaterial();

	void SetFurTexture(const wstring& assetFile);
	void SetColorTexture(const wstring& assetFile);
	void SetFinTexture(const wstring& assetFile);
	void SetNoiseTexture(const wstring& assetFile);

	float GetNrOfLayers()			{ return m_NrOfLayers; }
	void SetMaxHair(float max)		{ m_MaxHairLength = max; }
	void SetNrOfLayers(float l)		{ m_NrOfLayers = l; }
	void SetCurrentLayer(float l)	{ m_CurrentLayer = l; }
	void SetComb(XMFLOAT3 comb)		{ m_Comb = comb; }
	void SetDensity(float d)		{ m_Density = d; }
	float& GetDensity()				{ return m_Density; }
	float& GetFinOpacity()			{ return m_FinOpacity; }

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:

	TextureData* m_pDiffuseTexture;
	TextureData* m_pColorTexture;
	TextureData* m_pFinTexture;
	TextureData* m_pNoiseTexture;
	float m_CurrentLayer;
	float m_MaxHairLength;
	float m_NrOfLayers;
	XMFLOAT3 m_Comb;
	float m_Density;
	float m_FinOpacity;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectShaderResourceVariable* m_pColorSRV;
	static ID3DX11EffectShaderResourceVariable* m_pFinSRV;
	static ID3DX11EffectShaderResourceVariable* m_pNoiseSRV;
	static ID3DX11EffectScalarVariable* m_pCurrentLayerSRV;
	static ID3DX11EffectScalarVariable* m_pMaxHairLengthSRV;
	static ID3DX11EffectVectorVariable* m_pCombSRV;
	static ID3DX11EffectScalarVariable* m_pDensityVar;
	static ID3DX11EffectScalarVariable* m_pFinOpacityVar;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	FurMaterial(const FurMaterial &obj);
	FurMaterial& operator=(const FurMaterial& obj);
};

