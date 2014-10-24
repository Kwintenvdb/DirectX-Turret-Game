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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "../Base/OverlordGame.h"
#include "../Content/ContentManager.h"
#include "../Helpers/VertexHelper.h"

PostProcessingMaterial::PostProcessingMaterial(wstring effectFile, unsigned int renderIndex, const wstring& technique)
	: m_IsInitialized(false),
	m_pEffect(nullptr),
	m_effectFile(effectFile),
	m_pInputLayoutSize(0),
	m_InputLayoutID(0),
	m_RenderIndex(renderIndex),
	m_pRenderTarget(nullptr),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr),
	m_NumVertices(0),
	m_NumIndices(0),
	m_VertexBufferStride(0),
	m_pTechnique(nullptr),
	m_TechniqueName(technique)
{
}


PostProcessingMaterial::~PostProcessingMaterial()
{
	SafeRelease(m_pInputLayout);
	m_pInputLayoutDescriptions.clear();
	SafeDelete(m_pRenderTarget);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if(!m_IsInitialized)
	{
		// Load Effect
		if(!m_pEffect)
			LoadEffect(gameContext, m_effectFile);

		m_NumIndices = 6;
		m_NumVertices = 4;
		m_VertexBufferStride = sizeof(VertexPosTex);
		// Create Vertex Buffer
		CreateVertexBuffer(gameContext);		
		// Create Index Buffer
		CreateIndexBuffer(gameContext);
		// Create RenderTarget
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);

		RENDERTARGET_DESC renderDesc;
		renderDesc.Height = OverlordGame::GetGameSettings().Window.Height;
		renderDesc.Width = OverlordGame::GetGameSettings().Window.Width;

		m_pRenderTarget->Create(renderDesc);

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const wstring& effectFile)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
	LoadEffectVariables();
	
	return true;
}

void PostProcessingMaterial::SetRenderIndex(unsigned int index)
{
	m_RenderIndex = index;
}

unsigned int PostProcessingMaterial::GetRenderIndex()
{
	return m_RenderIndex;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	// Clear the render target
	float temp[] = {0,0,0,0};
	m_pRenderTarget->Clear(gameContext, temp); 
	// update the effect variables
	UpdateEffectVariables(previousRendertarget);
	// set input layout
	//EffectHelper::BuildInputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	// set vertex buffer
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	// set index buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// draw
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0 ,0);
    }
	
	// generate mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());


	// call PsSetShaderResources to reset the shader resources
	ID3D11ShaderResourceView *const pSRV[1] = {NULL};
    gameContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	


}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	// create a vertex buffer for a full screen quad using VertexPosTex
	if(!m_pVertexBuffer)
		{
			
			D3D11_BUFFER_DESC bd = {};
			bd.Usage =  D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			vector<VertexPosTex> vertices;
			vertices.push_back(VertexPosTex(XMFLOAT3(-1,-1,0), XMFLOAT2(0,1)));
			vertices.push_back(VertexPosTex(XMFLOAT3(-1,1,0), XMFLOAT2(0,0)));
			vertices.push_back(VertexPosTex(XMFLOAT3(1,1,0), XMFLOAT2(1,0)));
			vertices.push_back(VertexPosTex(XMFLOAT3(1,-1,0), XMFLOAT2(1,1)));

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = vertices.data();

			auto hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
			Logger::LogHResult(hr, L"PostProcessingMaterial::Initialize() :: can't create vertex buffer");

		}
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	// Create the index buffer for a full screen quad
	if(!m_pIndexBuffer)
		{
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(DWORD) * m_NumIndices;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			vector<DWORD> indices;
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(3);

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = indices.data();

			auto hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
			Logger::LogHResult(hr, L"PostProcessingMaterial::Initialize() :: can't create index buffer");
		}
}

RenderTarget*  PostProcessingMaterial::GetRenderTarget()
{
	return m_pRenderTarget;
}