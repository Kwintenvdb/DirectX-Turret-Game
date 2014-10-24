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

#include "TorusPrefab.h"
#include "../Components/Components.h"


TorusPrefab::TorusPrefab(float majorRadius, UINT majorRadiusSteps, float minorRadius, UINT minorRadiusSteps, XMFLOAT4 color):
		m_MajorRadius(majorRadius), 
		m_MinorRadius (minorRadius),
		m_MajorRadiusSteps(majorRadiusSteps),
		m_MinorRadiusSteps(minorRadiusSteps),
		m_Color(color)
{
}


TorusPrefab::~TorusPrefab(void)
{
}

void TorusPrefab::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Create Cube
	auto vertCount = m_MajorRadiusSteps * m_MinorRadiusSteps;
	auto mesh = new MeshIndexedDrawComponent(vertCount, vertCount*6);
	
	//**TORUS**

	//Vertices
	//vector<MeshDrawVertex> vertices;
	float majorInterval = XM_2PI/m_MajorRadiusSteps;
	float minorInterval = XM_2PI/m_MinorRadiusSteps; 
	for(UINT majorStep = 0; majorStep < m_MajorRadiusSteps; ++majorStep)
	{
		float torusRadius = majorStep * majorInterval;

		for(UINT minorStep = 0; minorStep < m_MinorRadiusSteps; ++minorStep)
		{
			float circleRadius = minorStep * minorInterval;
			XMFLOAT3 pos;
			pos.x = (m_MajorRadius + (m_MinorRadius*cos(circleRadius))) * cos(torusRadius);
			pos.y = (m_MajorRadius + (m_MinorRadius*cos(circleRadius))) * sin(torusRadius);
			pos.z = m_MinorRadius*sin(circleRadius);

			//Normal
			XMFLOAT3 middle;
			middle.x = m_MajorRadius * cos(torusRadius);
			middle.y = m_MajorRadius * sin(torusRadius);
			middle.z = 0;

			XMVECTOR vecPos = XMLoadFloat3(&pos);
			XMVECTOR vecMiddle = XMLoadFloat3(&middle);
			XMVECTOR vecNormal = XMVector3Normalize(XMVectorSubtract(vecPos, vecMiddle));
			XMFLOAT3 normal;
			XMStoreFloat3(&normal, vecNormal);

			mesh->AddVertex(VertexPosNormCol(pos, normal, m_Color));
			//vertices.push_back(MeshDrawVertex(pos, normal, m_Color));
		}
	}
	
	//Indices
	//vector<DWORD> indices;
	for(DWORD i = 0; i < (DWORD)vertCount; ++i)
	{
		DWORD v0, v1, v2, v3;
		v0 = i;
		v1 = (v0 + m_MinorRadiusSteps)%vertCount;
		v2 = v1 + 1;
		v3 = i + 1;

		if((i+1)%m_MinorRadiusSteps == 0)
		{
			v2 -= m_MinorRadiusSteps;
			v3 -= m_MinorRadiusSteps;
		}

		
		mesh->AddIndex(v0);
		mesh->AddIndex(v1);
		mesh->AddIndex(v2);
		mesh->AddIndex(v2);
		mesh->AddIndex(v3);
		mesh->AddIndex(v0);
	}

	AddComponent(mesh);
}
