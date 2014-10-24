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
#include "GameTime.h"
#include "../Components/CameraComponent.h"
#include "InputManager.h"
#include "MaterialManager.h"

struct GameSettings
{
public:
	GameSettings():
		Window(WindowSettings()),
		DirectX(DirectXSettings())
	{}

#pragma region
	struct WindowSettings
	{
		WindowSettings():
			Width(1280),
			Height(720),
			AspectRatio(Width/(float)Height),
			Title(L"Overlord Engine (DX11)"),
			WindowHandle(nullptr)
		{
		}

		int Width;
		int Height;
		float AspectRatio;
		wstring Title;
		HWND WindowHandle;
	}Window;
#pragma endregion WINDOW_SETTINGS

#pragma region
	struct DirectXSettings
	{
		DirectXSettings():
			pAdapter(nullptr),
			pOutput(nullptr)
		{}

		IDXGIAdapter* pAdapter;
		IDXGIOutput* pOutput;
	}DirectX;
#pragma endregion DIRECTX_SETTINGS
};

struct GameContext
{
public:
	GameTime* pGameTime;
	CameraComponent* pCamera;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	InputManager* pInput;
	MaterialManager* pMaterialManager;
};

enum CollisionGroupFlag : UINT32
{
	Group0 = (1 << 0),
	Group1 = (1 << 1),
	Group2 = (1 << 2),
	Group3 = (1 << 3),
	Group4 = (1 << 4),
	Group5 = (1 << 5),
	Group6 = (1 << 6),
	Group7 = (1 << 7),
	Group8 = (1 << 8),
	Group9 = (1 << 9)
};
