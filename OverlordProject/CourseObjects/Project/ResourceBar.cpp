#include "Base\stdafx.h"
#include "Base\OverlordGame.h"

#include "Components\Components.h"
#include "Physx/PhysxManager.h"
#include "ResourceBar.h"
#include "Diagnostics\Logger.h"
#include "Scenegraph/GameScene.h"

ResourceBar::ResourceBar(wstring texture, wstring text, float yOffset) :
	m_BarObj(nullptr),
	m_TextObj(nullptr),
	m_TextFile(L"./Resources/Textures/" + text + L".png"),
	m_TextureFile(L"./Resources/Textures/" + texture + L".png"),
	m_YOffset(yOffset)
{
}

ResourceBar::~ResourceBar(void)
{

}

void ResourceBar::Initialize(const GameContext& gameContext)
{
	m_BarObj = new GameObject();
	auto sprite = new SpriteComponent(m_TextureFile);
	m_BarObj->AddComponent(sprite);
	sprite->SetPivot(XMFLOAT2(0.5f, 0.5f));

	float offset = 20;
	float x = OverlordGame::GetGameSettings().Window.Width / 2;
	float y = OverlordGame::GetGameSettings().Window.Height - offset * m_YOffset;
	m_BarObj->GetTransform()->Translate(x,y,0.5f);
	AddChild(m_BarObj);

	m_TextObj = new GameObject();
	auto text = new SpriteComponent(m_TextFile);
	text->SetPivot(XMFLOAT2(0.5f,0.5f));
	m_TextObj->AddComponent(text);
	y -= offset + 2;
	m_TextObj->GetTransform()->Translate(x,y,0);
	AddChild(m_TextObj);
}

void ResourceBar::Update(const GameContext& gameContext)
{
	
}							   

void ResourceBar::Draw(const GameContext& gameContext)
{

}

void ResourceBar::SetResource(float amount, float max)
{
	float scale = amount / max;
	m_BarObj->GetTransform()->Scale(scale,1,0);
}