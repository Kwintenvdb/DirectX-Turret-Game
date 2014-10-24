#include "Base\stdafx.h"

#include "GrayMaterial.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"
#include "Graphics/RenderTarget.h"

ID3DX11EffectShaderResourceVariable* GrayMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectScalarVariable* GrayMaterial::m_pDesaturationSRV = nullptr;

GrayMaterial::GrayMaterial() : PostProcessingMaterial(L"./Resources/Effects/Grayscale.fx"), m_Desaturation()
{
}


GrayMaterial::~GrayMaterial()
{

}

void GrayMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"GrayMaterial::LoadEffectVariables() > \'gTexture\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	if(!m_pDesaturationSRV)
	{
		m_pDesaturationSRV = m_pEffect->GetVariableByName("Desaturation")->AsScalar();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"GrayMaterial::LoadEffectVariables() > \'Desaturation\' variable not found!");
			m_pDesaturationSRV = nullptr;
		}
	}
}

void GrayMaterial::UpdateEffectVariables(RenderTarget* rendertarget)
{
	if (m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(rendertarget->GetShaderResourceView());
	}

	if(m_pDesaturationSRV)
	{
		m_pDesaturationSRV->SetFloat(m_Desaturation);
	}
}