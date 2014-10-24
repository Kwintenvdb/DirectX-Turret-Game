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
#pragma once
#include "ContentLoader.h"

class PxTriangleMeshLoader : public ContentLoader<PxTriangleMesh>
{
public:
	PxTriangleMeshLoader(void);
	~PxTriangleMeshLoader(void);

protected:

	virtual PxTriangleMesh* LoadContent(const wstring& assetFile);
	virtual void Destroy(PxTriangleMesh* objToDestroy);

private:
// -------------------------
// Disabling default copy constructor and default 
// assignment operator.
// -------------------------
PxTriangleMeshLoader(const PxTriangleMeshLoader& yRef);									
PxTriangleMeshLoader& operator=(const PxTriangleMeshLoader& yRef);
};

