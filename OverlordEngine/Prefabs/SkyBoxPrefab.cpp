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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkyBoxPrefab.h"
#include "../Components/Components.h"
#include "../Graphics/Material.h"
#include "../../OverlordProject/Materials/SkyBoxMaterial.h"

SkyBoxPrefab::SkyBoxPrefab()
{
}


SkyBoxPrefab::~SkyBoxPrefab(void)
{
}

void SkyBoxPrefab::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Create Cube
	auto model = new ModelComponent(L"./Resources/Meshes/Box.ovm");
	
	//Create Material
	SkyBoxMaterial *material = new SkyBoxMaterial();
	material->SetDiffuseTexture(L"./Resources/Textures/SkyBox2.dds");
	gameContext.pMaterialManager->AddMaterial(material, 666);

	model->SetMaterial(666);

	AddComponent(model);
}
