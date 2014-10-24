//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MeshDrawComponent.h"
#include "../Content/ContentManager.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "../Base/GeneralStructs.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"

ID3DX11EffectMatrixVariable* MeshDrawComponent::m_pWorldVar = nullptr;
ID3DX11EffectMatrixVariable* MeshDrawComponent::m_pWvpVar = nullptr;

MeshDrawComponent::MeshDrawComponent(UINT triangleCapacity):
	BaseComponent(),
	m_TriangleCapacity(triangleCapacity),
	m_pVertexBuffer(nullptr),
	m_pInputLayout(nullptr),
	m_pEffect(nullptr),
	m_pTechnique(nullptr)
{
}


MeshDrawComponent::~MeshDrawComponent(void)
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void MeshDrawComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);

	InitializeBuffer(gameContext);
	UpdateBuffer();
}

void MeshDrawComponent::LoadEffect(const GameContext& gameContext)
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
	Logger::LogHResult(result, L"MeshDrawComponent::LoadEffect(...)");

	if (!m_pWorldVar)
		m_pWorldVar = m_pEffect->GetVariableBySemantic("World")->AsMatrix();

	if (!m_pWvpVar)
		m_pWvpVar = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
}

void MeshDrawComponent::InitializeBuffer(const GameContext& gameContext)
{
	if(m_pVertexBuffer)
		SafeRelease(m_pVertexBuffer);

	//*************
	//VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.BindFlags = D3D10_BIND_FLAG::D3D10_BIND_VERTEX_BUFFER;
	vertexBuffDesc.ByteWidth = sizeof(TrianglePosNormCol) * m_TriangleCapacity;
	vertexBuffDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.MiscFlags = 0;
	gameContext.pDevice->CreateBuffer(&vertexBuffDesc, NULL, &m_pVertexBuffer);
}

void MeshDrawComponent::UpdateBuffer()
{

#if _DEBUG
	auto scene = m_pGameObject->GetScene();	
	if(!scene)
	{
		Logger::LogWarning(L"MeshDrawComponent::UpdateBuffer > Can't update buffer, Component is not part of a scene. (= No DeviceContext)");
		return;
	}
#endif

	GameContext gameContext = m_pGameObject->GetScene()->GetGameContext();
	auto size = m_vecTriangles.size();

	if(size==0)return;
	if(size > m_TriangleCapacity)
	{
		Logger::LogInfo(L"MeshDrawComponent::UpdateBuffer > Buffer size clamped. (Increase TriangleCapacity)");
		size = m_TriangleCapacity;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, m_vecTriangles.data(), sizeof(TrianglePosNormCol) * size);
	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void MeshDrawComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void MeshDrawComponent::Draw(const GameContext& gameContext)
{
	//Set Shader Variables
	if(m_vecTriangles.size() == 0)
		return;
	 
	auto world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());

	m_pWorldVar->SetMatrix(reinterpret_cast<float*>(&world));
	XMMATRIX wvp = world*viewProjection;
	m_pWvpVar->SetMatrix(reinterpret_cast<float*>(&wvp));

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT offset = 0;
	UINT stride = sizeof(VertexPosNormCol);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_vecTriangles.size()*3,0); 
    }
}

void MeshDrawComponent::AddQuad(VertexPosNormCol vertex1, VertexPosNormCol vertex2, VertexPosNormCol vertex3, VertexPosNormCol vertex4, bool updateBuffer)
{
	AddTriangle(TrianglePosNormCol(vertex1,vertex2,vertex3), false);
	AddTriangle(TrianglePosNormCol(vertex3,vertex4,vertex1), updateBuffer);
}

void MeshDrawComponent::AddQuad(QuadPosNormCol quad, bool updateBuffer)
{
	AddTriangle(TrianglePosNormCol(quad.Vertex1,quad.Vertex2,quad.Vertex3), false);
	AddTriangle(TrianglePosNormCol(quad.Vertex3,quad.Vertex4,quad.Vertex1), updateBuffer);
}

void MeshDrawComponent::AddTriangle(VertexPosNormCol vertex1, VertexPosNormCol vertex2, VertexPosNormCol vertex3, bool updateBuffer)
{
	AddTriangle(TrianglePosNormCol(vertex1,vertex2,vertex3), updateBuffer);
}

void MeshDrawComponent::AddTriangle(TrianglePosNormCol triangle, bool updateBuffer)
{
	m_vecTriangles.push_back(triangle);

	//Logger::LogFixMe(L"MeshDrawComponent::AddTriangle > Fix buffer update");
	if(updateBuffer && m_IsInitialized)
		UpdateBuffer();
}

void MeshDrawComponent::RemoveTriangles()
{
	//Logger::LogFixMe(L"MeshDrawComponent::RemoveTriangles > Fix buffer update");
	m_vecTriangles.clear();
	//UpdateBuffer();
}
