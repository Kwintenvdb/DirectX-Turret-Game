#include "Base\stdafx.h"

#include "BlurMaterial.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"
#include "Graphics/RenderTarget.h"

ID3DX11EffectShaderResourceVariable* BlurMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectScalarVariable* BlurMaterial::m_pIterationsVar = nullptr;

BlurMaterial::BlurMaterial() : PostProcessingMaterial(L"./Resources/Effects/Blur.fx"),
	m_Iterations(1)
{
}


BlurMaterial::~BlurMaterial()
{

}

void BlurMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"BlurMaterial::LoadEffectVariables() > \'gTexture\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	if (!m_pIterationsVar)
	{
		m_pIterationsVar = m_pEffect->GetVariableByName("Iterations")->AsScalar();
		if (!m_pIterationsVar->IsValid())
		{
			Logger::LogWarning(L"BlurMaterial::LoadEffectVariables() > \'Iterations\' variable not found!");
			m_pIterationsVar = nullptr;
		}
	}
}

void BlurMaterial::UpdateEffectVariables(RenderTarget* rendertarget)
{
	if (m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(rendertarget->GetShaderResourceView());
	}

	if(m_pIterationsVar)
	{
		m_pIterationsVar->SetInt(m_Iterations);
	}
}