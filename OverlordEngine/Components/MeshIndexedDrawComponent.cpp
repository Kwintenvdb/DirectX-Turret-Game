//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MeshIndexedDrawComponent.h"
#include "../Content/ContentManager.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "../Base/GeneralStructs.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"

ID3DX11EffectMatrixVariable* MeshIndexedDrawComponent::m_pWorldVar = nullptr;
ID3DX11EffectMatrixVariable* MeshIndexedDrawComponent::m_pWvpVar = nullptr;

MeshIndexedDrawComponent::MeshIndexedDrawComponent(UINT vertexCapacity, UINT indexCapacity):
	m_vecVertices(vector<VertexPosNormCol>()),
	m_vecIndices(vector<UINT>()),
	m_VertexCapacity(vertexCapacity),
	m_IndexCapacity(indexCapacity),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr),
	m_pInputLayout(nullptr),
	m_pEffect(nullptr),
	m_pTechnique(nullptr)
{
}


MeshIndexedDrawComponent::~MeshIndexedDrawComponent(void)
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void MeshIndexedDrawComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);

	InitializeVertexBuffer(gameContext);
	InitializeIndexBuffer(gameContext);

	UpdateVertexBuffer();
	UpdateIndexBuffer();
}

void MeshIndexedDrawComponent::LoadEffect(const GameContext& gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/PosNormCol3D.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//*****************
	//Load Input Layout (TODO: EffectUtils::CreateInputLayout(...) +> use @ Material)
	D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = sizeof(layout)/sizeof(layout[0]);

	D3DX11_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	auto result = gameContext.pDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pInputLayout);
	Logger::LogHResult(result, L"MeshIndexedDrawComponent::LoadEffect(...)");

	if (!m_pWorldVar)
		m_pWorldVar = m_pEffect->GetVariableBySemantic("World")->AsMatrix();

	if (!m_pWvpVar)
		m_pWvpVar = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
}

void MeshIndexedDrawComponent::InitializeVertexBuffer(const GameContext& gameContext)
{
	if(m_pVertexBuffer)
		SafeRelease(m_pVertexBuffer);

	//*************
	//VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.ByteWidth = sizeof(VertexPosNormCol) * m_VertexCapacity;
	vertexBuffDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.MiscFlags = 0;
	gameContext.pDevice->CreateBuffer(&vertexBuffDesc, NULL, &m_pVertexBuffer);
}

void MeshIndexedDrawComponent::InitializeIndexBuffer(const GameContext& gameContext)
{
	if(m_pIndexBuffer)
		SafeRelease(m_pIndexBuffer);

	//*************
	//INDEX BUFFER
	D3D11_BUFFER_DESC indexBufDesc;
	indexBufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.ByteWidth = sizeof(UINT) * m_IndexCapacity;
	indexBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	indexBufDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	indexBufDesc.MiscFlags = 0;
	gameContext.pDevice->CreateBuffer(&indexBufDesc, NULL, &m_pIndexBuffer);
}

void MeshIndexedDrawComponent::UpdateVertexBuffer()
{

#if _DEBUG
	auto scene = m_pGameObject->GetScene();	
	if(!scene)
	{
		Logger::LogWarning(L"MeshIndexedDrawComponent::UpdateVertexBuffer > Can't update buffer, Component is not part of a scene. (= No DeviceContext)");
		return;
	}
#endif

	GameContext gameContext = m_pGameObject->GetScene()->GetGameContext();
	auto size = m_vecVertices.size();

	if(size==0)return;
	if(size > m_VertexCapacity)
	{
		Logger::LogInfo(L"MeshIndexedDrawComponent::UpdateVertexBuffer > Buffer size clamped. (Increase VertexCapacity)");
		size = m_VertexCapacity;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, m_vecVertices.data(), sizeof(VertexPosNormCol) * size);
	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void MeshIndexedDrawComponent::UpdateIndexBuffer()
{

#if _DEBUG
	auto scene = m_pGameObject->GetScene();	
	if(!scene)
	{
		Logger::LogWarning(L"MeshIndexedDrawComponent::UpdateIndexBuffer > Can't update buffer, Component is not part of a scene. (= No DeviceContext)");
		return;
	}
#endif

	GameContext gameContext = m_pGameObject->GetScene()->GetGameContext();
	auto size = m_vecIndices.size();

	if(size==0)return;
	if(size > m_IndexCapacity)
	{
		Logger::LogInfo(L"MeshIndexedDrawComponent::UpdateIndexBuffer > Buffer size clamped. (Increase IndexCapacity)");
		size = m_IndexCapacity;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pIndexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, m_vecIndices.data(), sizeof(UINT) * size);
	gameContext.pDeviceContext->Unmap(m_pIndexBuffer, 0);
}

void MeshIndexedDrawComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void MeshIndexedDrawComponent::Draw(const GameContext& gameContext)
{
	//Set Shader Variables
	if(m_vecIndices.size() == 0)
		return;
	 
	auto world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());

	m_pWorldVar->SetMatrix(reinterpret_cast<float*>(&world));

	XMMATRIX wvp = world*viewProjection;
	m_pWvpVar->SetMatrix(reinterpret_cast<float*>(&(wvp)));

	//Set Vertexbuffer
	UINT offset = 0;
	UINT stride = sizeof(VertexPosNormCol);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set Indexbuffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the input layout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	
	//Set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	

    D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_vecIndices.size(), 0, 0); 
    }
}

void MeshIndexedDrawComponent::AddVertex(VertexPosNormCol vertex, bool updateBuffer)
{
	m_vecVertices.push_back(vertex);

	if(updateBuffer)
		UpdateVertexBuffer();
}

void MeshIndexedDrawComponent::AddIndex(UINT index, bool updateBuffer)
{
	m_vecIndices.push_back(index);

	if(updateBuffer)
		UpdateIndexBuffer();
}

void MeshIndexedDrawComponent::ClearVertexList()
{
	m_vecVertices.clear();
	UpdateVertexBuffer();
}

void MeshIndexedDrawComponent::ClearIndexList()
{
	m_vecIndices.clear();
	UpdateIndexBuffer();
}

