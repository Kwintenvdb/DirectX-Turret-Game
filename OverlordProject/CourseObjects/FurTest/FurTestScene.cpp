#include "Base\stdafx.h"

#include "FurTestScene.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"

#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Helpers\PhysxHelper.h"
#include "Scenegraph\SceneManager.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/FurMaterial.h"
#include "Content/ContentManager.h"
#include "Graphics\TextRenderer.h"
#include <memory>

enum Input : int
{
	PlusMaxHair = 100,
	MinMaxHair,
	PlusLayers,
	MinLayers,
	PlusDens,
	MinDens,
	PlusFins,
	MinFins,
	PlusComb,
	MinComb,
	Sway,
	Rotate,
	CycleFur,
	ToggleText
};

FurTestScene::FurTestScene(void):
	GameScene(L"FurTestScene"),
	m_pFurObj(nullptr),
	m_pFur(nullptr),
	m_pFont(nullptr),
	m_MaxHair(1),
	m_Layers(60),
	m_SetSway(false),
	m_Rotation(),
	m_bRotate(false),
	m_CurrentFur(),
	m_UpdateText(1),
	m_ShowText(true)
{
	m_Comb = XMFLOAT3(0,0,0);
	m_FurTextures.push_back(L"./Resources/Textures/furcolor.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor3.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor4.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor5.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor6.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor7.png");
	m_FurTextures.push_back(L"./Resources/Textures/furcolor8.png");
}


FurTestScene::~FurTestScene(void)
{
}

void FurTestScene::Initialize(const GameContext& gameContext)
{
	// FUR TEST
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/font.fnt");

	auto g2 = new GameObject();
	auto m2 = new ModelComponent(L"./Resources/Meshes/Plane.ovm");
	auto colMat = new ColorMaterial();
	colMat->SetColor(XMFLOAT4(0.12f,0.12f,0.12f,1));
	m2->SetMaterial(colMat);
	g2->AddComponent(m2);
	AddChild(g2);
	g2->GetTransform()->Scale(0.01f,0.01f,0.01f);

	m_pFurObj = new GameObject();
	//auto mod = new ModelComponent(L"./Resources/Meshes/Plane.ovm");
	auto mod = new ModelComponent(L"./Resources/Meshes/cat.ovm");
	//go->GetTransform()->Scale(0.1f,0.1f,0.1f);
	m_pFur = new FurMaterial(m_MaxHair, m_Layers);
	m_pFur->SetFurTexture(L"./Resources/Textures/furTex.png");
	m_pFur->SetColorTexture(m_FurTextures[m_CurrentFur]);
	m_pFur->SetFinTexture(L"./Resources/Textures/FurTextureFin.dds");
	m_pFur->SetNoiseTexture(L"./Resources/Textures/noise.png");
	m_pFur->SetComb(m_Comb);
	//mod->SetMaterial(new ColorMaterial());
	mod->SetMaterial(m_pFur);
	m_pFurObj->AddComponent(mod);
	m_pFurObj->GetTransform()->Scale(0.2f,0.2f,0.2f);
	AddChild(m_pFurObj);

	CreateInputs(gameContext);
}

void FurTestScene::Update(const GameContext& gameContext)
{
	float x(0);
	if(m_bRotate || m_SetSway) x = (sin(gameContext.pGameTime->GetTotal())) * 1.2f;
	if(m_SetSway)
		m_Comb = XMFLOAT3(x * 0.65f, x * 0.65f, 0);

	if(m_bRotate)
		m_Rotation = x * 60;

	// INPUT
	float grow = 0.1f;
	float dens = 0.2f;
	float layers = 5;
	float comb = 0.035f;

	if(gameContext.pInput->IsActionTriggered(PlusMaxHair))
		m_MaxHair += grow;
	if(gameContext.pInput->IsActionTriggered(MinMaxHair))
		m_MaxHair -= grow;
	if(gameContext.pInput->IsActionTriggered(PlusLayers))
		m_Layers += layers;
	if(gameContext.pInput->IsActionTriggered(MinLayers))
		m_Layers -= layers;
	if(gameContext.pInput->IsActionTriggered(PlusDens))
		m_pFur->GetDensity() += dens;
	if(gameContext.pInput->IsActionTriggered(MinDens))
		m_pFur->GetDensity() -= dens;
	if(gameContext.pInput->IsActionTriggered(PlusFins))
		m_pFur->GetFinOpacity() += grow;
	if(gameContext.pInput->IsActionTriggered(MinFins))
		m_pFur->GetFinOpacity() -= grow;
	if(gameContext.pInput->IsActionTriggered(PlusComb) && !m_SetSway)
		m_Comb.x += comb;
	if(gameContext.pInput->IsActionTriggered(MinComb) && !m_SetSway)
		m_Comb.x -= comb;
	if(gameContext.pInput->IsActionTriggered(Sway))
		m_SetSway = !m_SetSway;
	if(gameContext.pInput->IsActionTriggered(Rotate))
		m_bRotate = !m_bRotate;
	if(gameContext.pInput->IsActionTriggered(CycleFur))
	{
		m_CurrentFur++;
		if(m_CurrentFur == m_FurTextures.size()) m_CurrentFur = 0;
		m_pFur->SetColorTexture(m_FurTextures[m_CurrentFur]);
	}
	if(gameContext.pInput->IsActionTriggered(ToggleText))
		m_ShowText = !m_ShowText;

	Clamp<float>(m_MaxHair, 10, 0);
	Clamp<float>(m_Layers, 250, 0);
	Clamp<float>(m_pFur->GetDensity(), 10, 1);
	Clamp<float>(m_pFur->GetFinOpacity(), 1, 0);
	Clamp<float>(m_Comb.x, 1.5f, -1.5f);
	m_pFur->SetMaxHair(m_MaxHair);
	m_pFur->SetNrOfLayers(m_Layers);
	m_pFur->SetComb(m_Comb);

	m_pFurObj->GetTransform()->Rotate(0, m_Rotation, 0);
}

void FurTestScene::Draw(const GameContext& gameContext)
{
	int fps = gameContext.pGameTime->GetFPS();
	float mil = gameContext.pGameTime->GetElapsed();

	m_UpdateText += mil;
	if(m_UpdateText > 0.25f)
	{
		m_OldFPS.str(L"");
		m_OldMil.str(L"");
		m_OldFPS << "FPS: " << fps;
		m_OldMil << "ms / frame: " << (int)(mil * 1000);

		m_UpdateText = 0;
	}

	auto inst = TextRenderer::GetInstance();

	//inst->DrawText(m_pFont, L"Press TAB to toggle text", XMFLOAT2(540,15), XMFLOAT4(1,1,1,0.3f));

	if(m_ShowText)
	{
		wstringstream info;
		info << "Hair length: " << m_MaxHair;
		inst->DrawText(m_pFont, info.str(), XMFLOAT2(15,80));
		info.str(L"");
		info << "Number of shells: " << m_Layers;
		inst->DrawText(m_pFont, info.str(), XMFLOAT2(15,102));
		info.str(L"");
		info << "Fur density: " << m_pFur->GetDensity() << " (best: 6 > 7)";
		inst->DrawText(m_pFont, info.str(), XMFLOAT2(15,124));
		info.str(L"");
		info << "Fin opacity: " << m_pFur->GetFinOpacity();
		inst->DrawText(m_pFont, info.str(), XMFLOAT2(15,146));

		float y = 528, o = 22;
		wstringstream controls;
		controls << "UP / DOWN: Adjust hair length";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y));
		controls.str(L"");
		controls << "O / L: Adjust number of shells";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o));
		controls.str(L"");
		controls << "I / K: Adjust fur density";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*2));
		controls.str(L"");
		controls << "U / J: Adjust fin opacity";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*3));
		controls.str(L"");
		controls << "LEFT / RIGHT: Adjust comb direction";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*4));
		controls.str(L"");
		controls << "C: Toggle sway comb";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*5));
		controls.str(L"");
		controls << "R: Toggle rotate mesh";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*6));
		controls.str(L"");
		controls << "N: Cycle fur patterns";
		inst->DrawText(m_pFont, controls.str(), XMFLOAT2(15,y+o*7));
			
		inst->DrawText(m_pFont, m_OldFPS.str(), XMFLOAT2(15,15));
		inst->DrawText(m_pFont, m_OldMil.str(), XMFLOAT2(15,37));
	}
}

void FurTestScene::CreateInputs(const GameContext& gameContext)
{
	auto inp = InputAction(PlusMaxHair, InputTriggerState::Released, VK_UP);
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(MinMaxHair, InputTriggerState::Released, VK_DOWN);
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(PlusLayers, InputTriggerState::Released, 'O');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(MinLayers, InputTriggerState::Released, 'L');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(PlusDens, InputTriggerState::Released, 'I');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(MinDens, InputTriggerState::Released, 'K');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(PlusFins, InputTriggerState::Released, 'U');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(MinFins, InputTriggerState::Released, 'J');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(PlusComb, InputTriggerState::Down, VK_RIGHT);
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(MinComb, InputTriggerState::Down, VK_LEFT);
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(Sway, InputTriggerState::Released, 'C');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(Rotate, InputTriggerState::Released, 'R');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(CycleFur, InputTriggerState::Released, 'N');
	gameContext.pInput->AddInputAction(inp);

	inp = InputAction(ToggleText, InputTriggerState::Released, VK_TAB);
	gameContext.pInput->AddInputAction(inp);
}