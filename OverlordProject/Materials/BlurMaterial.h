#pragma once
#include "Graphics\PostProcessingMaterial.h"

class RenderTarget;

class BlurMaterial : public PostProcessingMaterial
{
public:
	BlurMaterial();
	~BlurMaterial();

	void SetIterations(int it) { m_Iterations = it; }

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget);

private:

	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectScalarVariable* m_pIterationsVar;

	int m_Iterations;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BlurMaterial(const BlurMaterial &obj);
	BlurMaterial& operator=(const BlurMaterial& obj);
};