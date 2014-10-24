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

#include "MaterialManager.h"
#include "..\Graphics\Material.h"
#include <algorithm>
#include "..\Diagnostics\Logger.h"


MaterialManager::MaterialManager():
	m_pMaterials(map<UINT, Material*>())
{
}


MaterialManager::~MaterialManager()
{
	//Delete Materials
	for (pair<UINT, Material*> materialEntry : m_pMaterials)
	{
		SafeDelete(materialEntry.second);
	}

	m_pMaterials.clear();
}

Material* MaterialManager::GetMaterial(UINT materialId)
{
	auto it = m_pMaterials.find(materialId);
	if (it == m_pMaterials.end())
	{
		Logger::LogFormat(LogLevel::Warning, L"MaterialManager::GetMaterial() > Material with ID %i not found!", materialId);
		return nullptr;
	}

	return it->second;
}

void MaterialManager::RemoveMaterial(UINT materialId)
{
	auto it = m_pMaterials.find(materialId);
	if (it == m_pMaterials.end())
	{
		Logger::LogFormat(LogLevel::Warning, L"MaterialManager::RemoveMaterial() > Material with ID %i not found!", materialId);
		return;
	}

	SafeDelete(it->second);
	m_pMaterials.erase(it);
}

void MaterialManager::AddMaterial(Material *pMaterial, UINT materialId)
{
#if _DEBUG
	if (pMaterial == nullptr)
	{
		Logger::LogError(L"MaterialManager::AddMaterial() > Material is NULL!");
		return;
	}

	for (pair<UINT, Material*> materialEntry : m_pMaterials)
	{
		if (materialEntry.first == materialId)
		{
			Logger::LogFormat(LogLevel::Warning, L"MaterialManager::AddMaterial() > MaterialID %i is already used!", materialId);
			return;
		}

		if (materialEntry.second == pMaterial)
		{
			Logger::LogFormat(LogLevel::Warning, L"MaterialManager::AddMaterial() > Material is already added to the manager (ID %i)!", materialId);
			return;
		}
	}
#endif

	m_pMaterials[materialId] = pMaterial;
}
