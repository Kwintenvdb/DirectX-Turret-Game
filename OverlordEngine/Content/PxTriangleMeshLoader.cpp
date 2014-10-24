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
#include "stdafx.h"
#include "PxTriangleMeshLoader.h"
#include "..\Physx\PhysxManager.h"


PxTriangleMeshLoader::PxTriangleMeshLoader(void)
{
}


PxTriangleMeshLoader::~PxTriangleMeshLoader(void)
{
}

PxTriangleMesh* PxTriangleMeshLoader::LoadContent(const wstring& assetFile)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	string buffer = string(assetFile.begin(), assetFile.end());
	auto inputStream  = PxDefaultFileInputData(buffer.c_str());
	auto triangleMesh = physX->createTriangleMesh(inputStream);

	return triangleMesh;
}

void PxTriangleMeshLoader::Destroy(PxTriangleMesh* objToDestroy)
{
	UNREFERENCED_PARAMETER(objToDestroy);
	//PhysX handles its own resources
}
