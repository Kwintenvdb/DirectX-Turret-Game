#pragma once
#include "Graphics\PostProcessingMaterial.h"

class TextureData;
class RenderTarget;

class GrayMaterial : public PostProcessingMaterial
{
public:
	GrayMaterial();
	~GrayMaterial();

	void SetDesaturation(float d) 
	{ 
		m_Desaturation = d;
		Clamp<float>(m_Desaturation,1,0);
	}

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget);

private:

	float m_Desaturation;

	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectScalarVariable* m_pDesaturationSRV;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GrayMaterial(const GrayMaterial &obj);
	GrayMaterial& operator=(const GrayMaterial& obj);
};