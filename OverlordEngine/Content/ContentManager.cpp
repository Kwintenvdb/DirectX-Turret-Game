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

#include "ContentManager.h"
#include "EffectLoader.h"
#include "MeshFilterLoader.h"
#include "PxConvexMeshLoader.h"
#include "PxTriangleMeshLoader.h"
#include "TextureDataLoader.h"
#include "SpriteFontLoader.h"

vector<BaseLoader*> ContentManager::m_Loaders = vector<BaseLoader*>();
ID3D11Device* ContentManager::m_pDevice = nullptr;
bool ContentManager::m_IsInitialized = false;

ContentManager::ContentManager()
{
}


ContentManager::~ContentManager(void)
{
}

void ContentManager::Release()
{
	for(BaseLoader *ldr:m_Loaders)
	{	
		ldr->Unload();
		SafeDelete(ldr);
	}

	m_Loaders.clear();
}

void ContentManager::Initialize(ID3D11Device* pDevice)
{
	if(!m_IsInitialized)
	{
		m_pDevice = pDevice;
		m_IsInitialized = true;
		AddLoader(new EffectLoader());
		AddLoader(new MeshFilterLoader());
		AddLoader(new TextureDataLoader());
		AddLoader(new PxTriangleMeshLoader());
		AddLoader(new PxConvexMeshLoader());
		AddLoader(new SpriteFontLoader());
	}
}

void ContentManager::AddLoader(BaseLoader* loader)
{ 
	for(BaseLoader *ldr:m_Loaders)
	{	
		if(ldr->GetType()==loader->GetType())
		{
			SafeDelete(loader);
			break;
		}
	}

	m_Loaders.push_back(loader);
	loader->SetDevice(m_pDevice);
}
