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

#include "SpherePrefab.h"
#include "../Components/Components.h"


SpherePrefab::SpherePrefab(float radius, int steps, XMFLOAT4 color) :
		m_Steps(steps),
		m_Radius(radius),
		m_Color(color)
{
}


SpherePrefab::~SpherePrefab(void)
{
}

void SpherePrefab::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Create Cube
	auto vertCount = m_Steps * (m_Steps-1) + 2;
	auto mesh = new MeshIndexedDrawComponent(vertCount, ((m_Steps-2)*m_Steps*6)+(6*m_Steps));
	
	//**SPHERE**

	//Vertices
	float deltaTheta = XM_PI/m_Steps;
	float deltaPhi = XM_2PI / m_Steps;
	float theta = 0;
	float phi = 0;

	//TOP
	mesh->AddVertex(VertexPosNormCol(XMFLOAT3(0,m_Radius,0), XMFLOAT3(0,1,0), m_Color));

	//SPHERE
	for (int i = 0; i < m_Steps - 1; ++i)
	{
		theta += deltaTheta;
		for (int j = 0; j < m_Steps; ++j)
		{
			phi += deltaPhi;
			XMFLOAT3 pos;
			pos.x = m_Radius * sin(theta) * cos(phi);
			pos.z = m_Radius * sin(theta) * sin(phi);
			pos.y = m_Radius * cos(theta);

			XMVECTOR vPos = XMLoadFloat3(&pos);
			XMFLOAT3 normal;
			XMStoreFloat3(&normal, XMVector3Normalize(vPos));

			mesh->AddVertex(VertexPosNormCol(pos, normal, m_Color));
		}
	}

	//BOTTOM
	mesh->AddVertex(VertexPosNormCol(XMFLOAT3(0, -m_Radius, 0), XMFLOAT3(0, -1, 0), m_Color));
	
	//Indices
	//TOP
	for (DWORD i = 1; i < (DWORD) m_Steps + 1; ++i)
	{
		mesh->AddIndex(i);

		DWORD v1 = i + 1;
		if (i%m_Steps == 0)
			v1 -= m_Steps;

		mesh->AddIndex(v1);
		mesh->AddIndex(0);
	}

	//MIDDLE
	for (DWORD i = 1; i < (DWORD) vertCount - 1 - m_Steps; ++i)
	{
		DWORD v0, v1, v2, v3;
		v0 = i;
		v1 = i + 1;

		if (i%m_Steps == 0)
			v1 -= m_Steps;

		v2 = v1 + m_Steps;
		v3 = v0 + m_Steps;

		mesh->AddIndex(v0);
		mesh->AddIndex(v1);
		mesh->AddIndex(v2);
		mesh->AddIndex(v2);
		mesh->AddIndex(v3);
		mesh->AddIndex(v0);
	}

	//BOTTOM
	for (DWORD i = vertCount - m_Steps - 1; i < (DWORD) vertCount - 1; ++i)
	{
		mesh->AddIndex(i);

		DWORD v1 = i + 1;
		if (i%m_Steps == 0)
			v1 -= m_Steps;

		mesh->AddIndex(v1);
		mesh->AddIndex(vertCount - 1);
	}


	AddComponent(mesh);
}
