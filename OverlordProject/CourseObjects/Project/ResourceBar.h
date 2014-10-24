#pragma once
#include "Scenegraph/GameObject.h"

class ResourceBar : public GameObject
{
public:
	ResourceBar(wstring texture, wstring text, float yOffset);
	~ResourceBar(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

	void SetResource(float amount, float max);

private:

	GameObject* m_BarObj, *m_TextObj;
	wstring m_TextureFile, m_TextFile;
	float m_YOffset;
};