#pragma once
#include "Scenegraph/GameObject.h"

class CubePrefab;
class Player;

class LevelPiece : public GameObject
{
public:
	LevelPiece(float width, float height, float depth, XMFLOAT4 color = (XMFLOAT4)Colors::White);
	~LevelPiece(void);

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

private:

	CubePrefab* m_pCube;
	float m_Width, m_Height, m_Depth;
	XMFLOAT4 m_Color;
};