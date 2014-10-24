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
#include "../Helpers/EffectHelper.h"

class RenderTarget;
struct GameContext;

class PostProcessingMaterial
{
public:
	PostProcessingMaterial(wstring effectFile, unsigned int renderIndex = 0, const wstring& technique = L"");
	virtual ~PostProcessingMaterial();

	virtual void Initialize(const GameContext& gameContext);

	virtual void SetRenderIndex(unsigned int index);
	virtual unsigned int GetRenderIndex();

	virtual RenderTarget*  GetRenderTarget();
	virtual void Draw(const GameContext& gameContext, RenderTarget* previousRendertarget);

protected:
	virtual bool LoadEffect(const GameContext& gameContext, const wstring& effectFile);
	virtual void LoadEffectVariables() = 0;
	virtual void UpdateEffectVariables(RenderTarget* rendertarget) = 0;

	void CreateVertexBuffer(const GameContext& gameContext);
	void CreateIndexBuffer(const GameContext& gameContext);

	bool m_IsInitialized;
	
	ID3D11InputLayout *m_pInputLayout;
	vector<ILDescription> m_pInputLayoutDescriptions;
	int m_pInputLayoutSize;

	wstring m_effectFile;
	UINT m_InputLayoutID;
	unsigned int m_RenderIndex;

	RenderTarget* m_pRenderTarget;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_NumVertices,m_NumIndices;	
	unsigned int m_VertexBufferStride;

	ID3DX11Effect *m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	wstring m_TechniqueName;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostProcessingMaterial(const PostProcessingMaterial& yRef);									
	PostProcessingMaterial& operator=(const PostProcessingMaterial& yRef);
};

