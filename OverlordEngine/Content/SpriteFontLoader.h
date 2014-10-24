#pragma once
#include "ContentLoader.h"
#include "../Graphics/SpriteFont.h"

class SpriteFontLoader : public ContentLoader<SpriteFont>
{
public:
	SpriteFontLoader(void);
	~SpriteFontLoader(void);

protected:
	virtual SpriteFont* LoadContent(const wstring& assetFile);
	virtual void Destroy(SpriteFont* objToDestroy);

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpriteFontLoader(const SpriteFontLoader& yRef);
	SpriteFontLoader& operator=(const SpriteFontLoader& yRef);
};

