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
#include "GeneralStructs.h"

class RenderTarget;
class GameTime;

class OverlordGame
{
public:
	OverlordGame(void);
	~OverlordGame(void);

	static LRESULT CALLBACK WindowsProcedureStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Run(HINSTANCE hInstance);

	//Methods
	static const GameSettings& GetGameSettings() { return m_GameSettings; }
	void SetRenderTarget(RenderTarget* renderTarget);
	// Update PP
	RenderTarget* GetRenderTarget();

protected:

	virtual void OnGamePreparing(GameSettings& gameSettings){ UNREFERENCED_PARAMETER(gameSettings); }
	virtual LRESULT WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void Initialize() = 0;
	//virtual void Update(GameContext context) = 0;
	//virtual void Draw(GameContext context)Set = 0;

private:

	//FUNCTIONS
	//Initializations
	HRESULT InitializeAdapterAndOutput();
	HRESULT InitializeWindow();
	HRESULT InitializeDirectX();
	HRESULT InitializePhysX();
	HRESULT InitializeGame();
	void GameLoop();

	//Windows Proc
	LRESULT WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//MEMBERS
	static GameSettings m_GameSettings;

	HINSTANCE m_hInstance;
	HWND m_WindowHandle;	
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapchain;
	IDXGIFactory* m_pDxgiFactory;
	RenderTarget* m_pDefaultRenderTarget, *m_pCurrentRenderTarget; 
	D3D11_VIEWPORT m_Viewport;
};

