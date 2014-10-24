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
#pragma once
#include "ContentLoader.h"
#include <unordered_map>
#include "..\Graphics\MeshFilter.h"

class MeshFilterLoader : public ContentLoader<MeshFilter>
{
public:
	MeshFilterLoader(void){};
	virtual ~MeshFilterLoader(void){};

protected:
	virtual MeshFilter* LoadContent(const wstring& assetFile);
	virtual void Destroy(MeshFilter* objToDestroy);

private:

	enum MeshDataType : int
	{
		END = 0,
		HEADER = 1,
		POSITIONS = 2,
		INDICES = 3,
		NORMALS = 4,
		BINORMALS = 5,
		TANGENTS = 6,		
		COLORS = 7,
		TEXCOORDS = 8,
		BLENDINDICES = 9,
		BLENDWEIGHTS = 10,
		ANIMATIONCLIPS = 11,
		SKELETON = 12
	};

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshFilterLoader(const MeshFilterLoader &obj);
	MeshFilterLoader& operator=(const MeshFilterLoader& obj);
};
