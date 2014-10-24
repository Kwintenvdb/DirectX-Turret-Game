#pragma once
#include "Scenegraph/GameObject.h"

class Terrain : public GameObject
{
public:
	Terrain();
	~Terrain(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

private:

};