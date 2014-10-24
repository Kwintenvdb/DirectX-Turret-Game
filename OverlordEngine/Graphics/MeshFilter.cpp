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

#include "MeshFilter.h"
#include "..\Graphics\Material.h"
#include "..\Diagnostics\Logger.h"
#include "..\Base\GeneralStructs.h"
#include <algorithm>

XMFLOAT4 MeshFilter::m_DefaultColor = XMFLOAT4(1,0,0,1);
XMFLOAT4 MeshFilter::m_DefaultFloat4 = XMFLOAT4(0, 0, 0, 0);
XMFLOAT3 MeshFilter::m_DefaultFloat3 = XMFLOAT3(0, 0, 0);
XMFLOAT2 MeshFilter::m_DefaultFloat2 = XMFLOAT2(0, 0);

MeshFilter::MeshFilter():
			m_Positions(vector<XMFLOAT3>()),
			m_Normals(vector<XMFLOAT3>()),
			m_Tangents(vector<XMFLOAT3>()),
			m_Binormals(vector<XMFLOAT3>()),
			m_TexCoords(vector<XMFLOAT2>()),
			m_Colors(vector<XMFLOAT4>()),
			m_Indices(vector<DWORD>()),
			m_BlendIndices(vector<XMFLOAT4>()),
			m_BlendWeights(vector<XMFLOAT4>()),
			m_AnimationClips(vector<AnimationClip>()),
			m_pIndexBuffer(nullptr),
			m_VertexBuffers(vector<const VertexBufferData>()),
			m_VertexCount(0),
			m_IndexCount(0),
			m_HasElement(NONE),
			m_HasAnimations(false),
			m_BoneCount(0)
{
}


MeshFilter::~MeshFilter()
{
	m_Positions.clear();
	m_Normals.clear();
	m_TexCoords.clear();
	m_Colors.clear();
	m_Indices.clear();
	m_Tangents.clear();
	m_Binormals.clear();
	m_BlendIndices.clear();
	m_BlendWeights.clear();
	m_AnimationClips.clear();

	for_each(m_VertexBuffers.begin(), m_VertexBuffers.end(), [](VertexBufferData& data)
	{
		data.Destroy();
	});

	m_VertexBuffers.clear();
	SafeRelease(m_pIndexBuffer);
}

void MeshFilter::BuildIndexBuffer(const GameContext& gameContext)
{
	if (m_pIndexBuffer != nullptr)
		return;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_Indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_Indices.data();

	auto hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"MeshFilter::BuildIndexBuffer()");
}

int MeshFilter::GetVertexBufferId(UINT inputLayoutId)
{
	for (UINT i = 0; i<m_VertexBuffers.size(); ++i)
	{
		if (m_VertexBuffers[i].InputLayoutID == inputLayoutId)
			return i;
	}

	return -1;
}

void MeshFilter::BuildVertexBuffer(const GameContext& gameContext, Material* pMaterial)
{
	//Check if VertexBufferInfo already exists with requested InputLayout
	if (GetVertexBufferId(pMaterial->m_InputLayoutID) >= 0)
		return;

	VertexBufferData data;
	data.VertexStride = pMaterial->m_pInputLayoutSize;
	data.VertexCount = m_VertexCount;
	data.BufferSize = data.VertexStride * m_VertexCount;
	data.IndexCount = m_IndexCount;

	void *pDataLocation = malloc(data.BufferSize);
	if (pDataLocation == nullptr)
	{
		Logger::LogWarning(L"MeshFilter::BuildVertexBuffer() > Failed to allocate the required memory!");
		return;
	}

	data.pDataStart = pDataLocation;
	data.InputLayoutID = pMaterial->m_InputLayoutID;

	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		for (UINT j = 0; j < pMaterial->m_pInputLayoutDescriptions.size(); ++j)
		{
			auto ilDescription = pMaterial->m_pInputLayoutDescriptions[j];

			if (i == 0 && !HasElement(ilDescription.SemanticType))
			{
				wstring name = EffectHelper::GetIlSemanticName(ilDescription.SemanticType);
				Logger::LogFormat(LogLevel::Warning, L"MeshFilter::BuildVertexBuffer > Mesh \"%s\" has no vertex %s data, using a default value!", m_MeshName.c_str(), name.c_str());
			}

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType)?&m_Positions[i]:&m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Normals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::COLOR:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Colors[i] : &m_DefaultColor, ilDescription.Offset);
				break;
			case ILSemantic::TEXCOORD:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_TexCoords[i] : &m_DefaultFloat2, ilDescription.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Tangents[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BINORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Binormals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BLENDINDICES:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendIndices[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			case ILSemantic::BLENDWEIGHTS:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendWeights[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			default:
				Logger::LogError(L"MeshFilter::BuildVertexBuffer() > Unsupported SemanticType!");
				break;
			}

			pDataLocation = (char *) pDataLocation + ilDescription.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = data.BufferSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data.pDataStart;
	//create a ID3D10Buffer in graphics memory containing the vertex info
	gameContext.pDevice->CreateBuffer(&bd, &initData, &data.pVertexBuffer);

	m_VertexBuffers.push_back(data);
}

const VertexBufferData& MeshFilter::GetVertexBufferData(const GameContext& gameContext, Material* pMaterial)
{
	int possibleBuffer = GetVertexBufferId(pMaterial->m_InputLayoutID);

	if (possibleBuffer < 0)
	{
		Logger::LogWarning(L"MeshFilter::GetVertexBufferInformation(...) => No VertexBufferInformation for this material found! Building matching VertexBufferInformation (Performance Issue).");
		BuildVertexBuffer(gameContext, pMaterial);

		//Return last created vertexbufferinformation
		return m_VertexBuffers.back();
	}

	return m_VertexBuffers[possibleBuffer];
}
