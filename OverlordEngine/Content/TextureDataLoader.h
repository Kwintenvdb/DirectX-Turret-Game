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
#include "..\Graphics\TextureData.h"

class TextureDataLoader : public ContentLoader<TextureData>
{
public:
	TextureDataLoader(void);
	~TextureDataLoader(void);

protected:

	virtual TextureData* LoadContent(const wstring& assetFile);
	virtual void Destroy(TextureData* objToDestroy);

private:
// -------------------------
// Disabling default copy constructor and default 
// assignment operator.
// -------------------------
TextureDataLoader(const TextureDataLoader& yRef);									
TextureDataLoader& operator=(const TextureDataLoader& yRef);
};

