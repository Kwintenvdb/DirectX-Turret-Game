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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DebugRenderer.h"
#include "../Base/GeneralStructs.h"
#include "../Helpers/VertexHelper.h"
#include "Logger.h"
#include "../Content/ContentManager.h"

ID3DX11Effect* DebugRenderer::m_pEffect = nullptr;
ID3DX11EffectTechnique* DebugRenderer::m_pTechnique = nullptr;
UINT DebugRenderer::m_BufferSize = 100;
ID3D11Buffer* DebugRenderer::m_pVertexBuffer = nullptr;
ID3D11InputLayout* DebugRenderer::m_pInputLayout = nullptr;
ID3DX11EffectMatrixVariable* DebugRenderer::m_pWvpVariable = nullptr;
vector<VertexPosCol> DebugRenderer::m_LineList = vector<VertexPosCol>();
vector<VertexPosCol> DebugRenderer::m_FixedLineList = vector<VertexPosCol>();
UINT DebugRenderer::m_FixedBufferSize = 0;
bool DebugRenderer::m_RendererEnabled = true;

DebugRenderer::DebugRenderer(void)
{
}


DebugRenderer::~DebugRenderer(void)
{
}

void DebugRenderer::Release()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void DebugRenderer::InitRenderer(ID3D11Device *pDevice, UINT bufferSize)
{
	m_BufferSize = bufferSize;
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/DebugRenderer.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	m_pWvpVariable = m_pEffect->GetVariableBySemantic("WORLDVIEWPROJECTION")->AsMatrix();

	if(!m_pWvpVariable->IsValid())
		Logger::LogWarning(L"Debug Renderer: Invalid Shader Variable! (WVP)");

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	pDevice->CreateInputLayout(layout, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);

	CreateFixedLineList();
	CreateVertexBuffer(pDevice);
}

void DebugRenderer::CreateVertexBuffer(ID3D11Device *pDevice)
{
	SafeRelease(m_pVertexBuffer);

	//Vertexbuffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(VertexPosCol) * (m_BufferSize + m_FixedBufferSize);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	
	//Crashes on some systems :s
	//D3D11_SUBRESOURCE_DATA initData = {0};
	//initData.pSysMem = m_FixedLineList.data();
	//pDevice->CreateBuffer(&buffDesc, &initData, &m_pVertexBuffer);

	//Solution: Create buffer without initial data
	pDevice->CreateBuffer(&buffDesc,nullptr,&m_pVertexBuffer);
	
	if(m_FixedBufferSize > 0)
	{
		//Map Fixed data manually
		ID3D11DeviceContext* pDeviceContext;
		pDevice->GetImmediateContext(&pDeviceContext);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_FixedLineList[0], sizeof(VertexPosCol) * m_FixedBufferSize);
		pDeviceContext->Unmap(m_pVertexBuffer, 0);
		pDeviceContext->Release();
	}
}

void DebugRenderer::CreateFixedLineList()
{
	//*GRID*
	UINT numGridLines = 20;
	float gridSpacing = 1.0f;

	float startOffset = -((int)numGridLines/2)*gridSpacing;
	float size = (numGridLines - 1) * gridSpacing;
	auto gridColor = (XMFLOAT4)Colors::LightGray;
	for(UINT i = 0; i < numGridLines; ++i)
	{
		//VERTICAL
		float lineOffset = startOffset + gridSpacing * i;
		auto vertStart = XMFLOAT3(startOffset, 0, lineOffset);
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
		vertStart.x += size;
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));

		//HORIZONTAL
		vertStart = XMFLOAT3(lineOffset, 0, startOffset);
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
		vertStart.z += size;
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
	}

	//*AXIS
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(0,0,0), (XMFLOAT4)Colors::DarkRed));
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(30,0,0), (XMFLOAT4)Colors::DarkRed));
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(0,0,0), (XMFLOAT4)Colors::DarkGreen));
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(0,30,0), (XMFLOAT4)Colors::DarkGreen));
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(0,0,0), (XMFLOAT4)Colors::DarkBlue));
	m_FixedLineList.push_back(VertexPosCol(XMFLOAT3(0,0,30), (XMFLOAT4)Colors::DarkBlue));

	//@END!
	m_FixedBufferSize = m_FixedLineList.size();
}

void DebugRenderer::ToggleDebugRenderer()
{
	m_RendererEnabled = !m_RendererEnabled;
}

void DebugRenderer::DrawLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT4 color)
{
	if(!m_RendererEnabled)
		return;

	m_LineList.push_back(VertexPosCol(start, color));
	m_LineList.push_back(VertexPosCol(end, color));
}

void DebugRenderer::DrawLine(XMFLOAT3 start, XMFLOAT4 colorStart, XMFLOAT3 end, XMFLOAT4 colorEnd)
{
	if(!m_RendererEnabled)
		return;

	m_LineList.push_back(VertexPosCol(start, colorStart));
	m_LineList.push_back(VertexPosCol(end, colorEnd));
}

void DebugRenderer::DrawPhysX(PxScene* pScene)
{
	if(!m_RendererEnabled)
		return;

	auto pxDebugRenderer = &pScene->getRenderBuffer();
	auto debugLines = pxDebugRenderer->getLines();

	for(UINT i = 0; i < pxDebugRenderer->getNbLines(); ++i)
	{
		auto line = debugLines[i];
		DrawLine(XMFLOAT3(line.pos0.x, line.pos0.y, line.pos0.z),ConvertPxColor(line.color0),XMFLOAT3(line.pos1.x, line.pos1.y, line.pos1.z),ConvertPxColor(line.color1));
	}
}

XMFLOAT4 DebugRenderer::ConvertPxColor(PxU32 color)
{
	//TODO: Check performance, Bitshift+divide vs switch
	switch(color)
	{
		case 0xFF000000:
			return (XMFLOAT4)Colors::Black;
		case 0xFFFF0000:
			return (XMFLOAT4)Colors::Red;
		case 0xFF00FF00:
			return (XMFLOAT4)Colors::Green;
		case 0xFF0000FF:
			return (XMFLOAT4)Colors::Blue;
		case 0xFFFFFF00:
			return (XMFLOAT4)Colors::Yellow;
		case 0xFFFF00FF:
			return (XMFLOAT4)Colors::Magenta;
		case 0xFF00FFFF:
			return (XMFLOAT4)Colors::Cyan;
		case 0xFFFFFFFF:
			return (XMFLOAT4)Colors::White;
		case 0xFF808080:
			return (XMFLOAT4)Colors::Gray;
		case 0x88880000:
			return (XMFLOAT4)Colors::DarkRed;
		case 0x88008800:
			return (XMFLOAT4)Colors::DarkGreen;
		case 0x88000088:
			return (XMFLOAT4)Colors::DarkBlue;
		default:
			return (XMFLOAT4)Colors::Black;
	}
}

void DebugRenderer::Draw(const GameContext& gameContext)
{
	if(!m_RendererEnabled)
		return;

	UINT size = m_LineList.size();

	if((size + m_FixedBufferSize) <= 0)
		return;
	else if(size > m_BufferSize)
	{
		Logger::LogInfo(L"DebugRenderer::Draw() > Increasing Vertexbuffer Size!");
		m_BufferSize = size;
		CreateVertexBuffer(gameContext.pDevice);
	}

	auto pDevContext = gameContext.pDeviceContext;

	if(size>0)
	{
		//Map Vertex Data
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pDevContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(&(((VertexPosCol*)mappedResource.pData)[m_FixedBufferSize]), &m_LineList[0], sizeof(VertexPosCol) * size);
		pDevContext->Unmap(m_pVertexBuffer, 0);
	}

	//Set Render Pipeline
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(VertexPosCol);
	UINT offset = 0;
	pDevContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevContext->IASetInputLayout(m_pInputLayout);

	auto viewProj = gameContext.pCamera->GetViewProjection();
	XMMATRIX wvp = XMMatrixIdentity() * XMLoadFloat4x4(&viewProj);
	XMFLOAT4X4 wvpConverted;
	XMStoreFloat4x4( &wvpConverted, wvp);

	m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&wvpConverted));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for(UINT i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, pDevContext);
		pDevContext->Draw(size + m_FixedBufferSize, 0);
	}
	
	m_LineList.clear();
}
