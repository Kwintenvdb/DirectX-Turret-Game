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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "OverlordGame.h"
#include "../Diagnostics/Logger.h"
#include "../Graphics/RenderTarget.h"
#include "../Scenegraph/SceneManager.h"
#include "GameTime.h"
#include "../Content/ContentManager.h"
#include "../Physx/PhysxManager.h"
#include "../Diagnostics/DebugRenderer.h"
#include "../Graphics/SpriteRenderer.h"
#include "../Graphics/TextRenderer.h"

GameSettings OverlordGame::m_GameSettings = GameSettings();

//FOR NVIDIA GPUS ONLY
//Force NVIDIA Optimus to use the NVIDIA GPU
//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

OverlordGame::OverlordGame(void):
	m_hInstance(nullptr),
	m_WindowHandle(nullptr),
	m_pDevice(nullptr),
	m_pDeviceContext(nullptr),
	m_pSwapchain(nullptr),
	m_pDxgiFactory(nullptr),
	m_pDefaultRenderTarget(nullptr),
	m_pCurrentRenderTarget(nullptr)
{
	Logger::Initialize();
}


OverlordGame::~OverlordGame(void)
{
	//GameSettings Cleanup
	SafeRelease(m_GameSettings.DirectX.pAdapter);
	SafeRelease(m_GameSettings.DirectX.pOutput);

	//Game Cleanup
	ContentManager::Release();
	DebugRenderer::Release();
	SpriteRenderer::DestroyInstance();
	TextRenderer::DestroyInstance();
	SceneManager::DestroyInstance();
	PhysxManager::DestroyInstance();
	Logger::Release();

	//DirectX Cleanup
	SafeDelete(m_pDefaultRenderTarget);
	SafeRelease(m_pDxgiFactory);
	SafeRelease(m_pSwapchain);

	if(m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		SafeRelease(m_pDeviceContext);
	}

	SafeRelease(m_pDevice);
	
}

HRESULT OverlordGame::Run(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	//PREPARATION
	//***********
	OnGamePreparing(m_GameSettings);

	//INITIALIZE
	//**********
	auto hr = InitializeAdapterAndOutput();
	if(Logger::LogHResult(hr, L"OverlordGame::InitializeAdapterAndOutput")) return hr;

	hr = InitializeWindow();
	if(Logger::LogHResult(hr, L"OverlordGame::InitializeWindow")) return hr;

	hr = InitializeDirectX();
	if(Logger::LogHResult(hr, L"OverlordGame::InitializeDirectX")) return hr;

	hr = InitializePhysX();
	if(Logger::LogHResult(hr, L"OverlordGame::InitializePhysX")) return hr;

	hr = InitializeGame();
	if(Logger::LogHResult(hr, L"OverlordGame::InitializeGame")) return hr;

	//GAMELOOP
	//********
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		GameLoop();
		//Sleep(100);
	}

	//ALL OK :)
	//TODO: should return 'msg.wParam'
	return S_OK;
}

#pragma region
HRESULT OverlordGame::InitializeAdapterAndOutput()
{
	auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&m_pDxgiFactory));
	if(FAILED(hr)) return hr;

	if(!m_GameSettings.DirectX.pAdapter)
	{	
		hr = m_pDxgiFactory->EnumAdapters(0, &m_GameSettings.DirectX.pAdapter);
		if(FAILED(hr)) return hr;
	}

	if(!m_GameSettings.DirectX.pOutput)
	{
		IDXGIOutput *tempOutput;
		hr = m_GameSettings.DirectX.pAdapter->EnumOutputs(0, &tempOutput);
		if(FAILED(hr)) return hr;

		hr = tempOutput->QueryInterface(__uuidof(IDXGIOutput),(void**)(&m_GameSettings.DirectX.pOutput));
		if(FAILED(hr)) return hr;
		SafeRelease(tempOutput);
	}

	return S_OK;
}

HRESULT OverlordGame::InitializeWindow()
{
	//1. Create Windowclass
	//*********************
	auto className = L"OverlordWindowClass";
	WNDCLASS windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASS));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowsProcedureStatic;
	windowClass.hInstance = m_hInstance;
	windowClass.lpszClassName = className;

	if(!RegisterClass(&windowClass))
	{
		auto error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	//2. Create Window
	//****************
	DXGI_OUTPUT_DESC outputDesc;
	auto hr = m_GameSettings.DirectX.pOutput->GetDesc(&outputDesc);
	if(FAILED(hr))return hr;

	RECT r = {0, 0, m_GameSettings.Window.Width, m_GameSettings.Window.Height};
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	auto winWidth = r.right - r.left;
	auto winHeight = r.bottom - r.top;

	int x = outputDesc.DesktopCoordinates.left + ((outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left)/2) - winWidth/2;
	int y = (outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top)/2 - winHeight/2;

	m_WindowHandle = CreateWindow(className,
									m_GameSettings.Window.Title.c_str(), 
									WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
									x, 
									y, 
									winWidth, 
									winHeight, 
									NULL, 
									0, 
									m_hInstance, 
									this);

	if(!m_WindowHandle)
	{
		auto error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	m_GameSettings.Window.WindowHandle = m_WindowHandle;

	//3. Show The Window
	//******************
	ShowWindow(m_WindowHandle, SW_SHOWDEFAULT);

	return S_OK;
}

HRESULT OverlordGame::InitializeDirectX()
{
	//Create DX11 Device & Context
	UINT createDeviceFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	auto hr = D3D11CreateDevice(m_GameSettings.DirectX.pAdapter,
								D3D_DRIVER_TYPE_UNKNOWN,
								NULL,
								createDeviceFlags,
								0,0,
								D3D11_SDK_VERSION,
								&m_pDevice,
								&featureLevel,
								&m_pDeviceContext);

	if(FAILED(hr))return hr;
	if(featureLevel < D3D_FEATURE_LEVEL_10_1)
	{
		Logger::LogHResult(-1, L"Feature level 10.1+ not supported on this device!");
		exit(-1);
	}
	else if (featureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		Logger::LogWarning(L"Feature level 10.1, some DirectX11 specific features won't be available on this device!");
	}

	////DEBUG TEST
	//ID3D11Debug *d3dDebug = nullptr;
	//if(SUCCEEDED( m_pDevice->QueryInterface( __uuidof(ID3D11Debug), (void**)&d3dDebug ) ) )
	//{
	//	ID3D11InfoQueue *d3dInfoQueue = nullptr;
	//	d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
	//
	//	if( SUCCEEDED( d3dDebug->QueryInterface( __uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue ) ) )
	//	{
	//		#ifdef _DEBUG
	//		d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_CORRUPTION, true );
	//		d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_ERROR, true );
	//		#endif
 //
	//		D3D11_MESSAGE_ID hide [] =
	//		{
	//		D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
	//		// Add more message IDs here as needed
	//		};
 //
	//		D3D11_INFO_QUEUE_FILTER filter;
	//		memset( &filter, 0, sizeof(filter) );
	//		filter.DenyList.NumIDs = _countof(hide);
	//		filter.DenyList.pIDList = hide;
	//		d3dInfoQueue->AddStorageFilterEntries( &filter );
	//		d3dInfoQueue->Release();
	//	}

	//	d3dDebug->Release();
	//}

	////Create Swapchain
	//DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	//ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	//swapChainDesc.Width = 0; //Inherit HWND width
	//swapChainDesc.Height = 0; //Inherit HWND Height
	//swapChainDesc.Stereo = false;
	//swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//DXGI_SAMPLE_DESC sampleDesc;
	//ZeroMemory(&sampleDesc, sizeof(DXGI_SAMPLE_DESC));
	//sampleDesc.Count = 1;
	//sampleDesc.Quality = 0;
	//swapChainDesc.SampleDesc = sampleDesc;

	//swapChainDesc.BufferCount = 1;
	//swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//|DXGI_USAGE_SHADER_INPUT; //Only for PP

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc;
	//ZeroMemory(&swapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//swapChainFullScreenDesc.Windowed = true;
	//swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//swapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//
	//DXGI_RATIONAL rational;
	//ZeroMemory(&rational, sizeof(DXGI_RATIONAL));
	//rational.Denominator = 1;
	//rational.Numerator = 60;
	//swapChainFullScreenDesc.RefreshRate = rational;

	//hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pDevice, m_WindowHandle, &swapChainDesc, &swapChainFullScreenDesc, NULL, &m_pSwapchain);

	//Create Swapchain descriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = m_GameSettings.Window.Width;
	swapChainDesc.BufferDesc.Height = m_GameSettings.Window.Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	// Update PP
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_WindowHandle;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	hr = m_pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapchain);
	if(FAILED(hr))	return hr;

	//Create the default rendertarget.
	m_pDefaultRenderTarget = new RenderTarget(m_pDevice);
	
	ID3D11Texture2D *pBackbuffer = nullptr;
	hr = m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));
	if(FAILED(hr)) return hr;

	RENDERTARGET_DESC rtDesc;
	rtDesc.pColor = pBackbuffer;
	hr = m_pDefaultRenderTarget->Create(rtDesc);
	if(FAILED(hr)) return hr;

	//Set Default Rendertarget 
	SetRenderTarget(nullptr);

	Logger::LogFixMe(L"Viewport ownership, overlordgame");
	m_Viewport.Width	= (FLOAT)m_GameSettings.Window.Width;
	m_Viewport.Height	= (FLOAT)m_GameSettings.Window.Height;
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1,&m_Viewport);

	return S_OK;
}

HRESULT OverlordGame::InitializePhysX()
{
	PhysxManager::GetInstance()->Init(m_pDevice);
	return S_OK;
}

HRESULT OverlordGame::InitializeGame()
{
	//******************
	//MANAGER INITIALIZE
	ContentManager::Initialize(m_pDevice);
	DebugRenderer::InitRenderer(m_pDevice);
	SpriteRenderer::GetInstance()->InitRenderer(m_pDevice);
	TextRenderer::GetInstance()->InitRenderer(m_pDevice);
	// Update PP
	SceneManager::GetInstance()->Initialize(m_pDevice, m_pDeviceContext, this);

	//***************
	//GAME INITIALIZE
	Initialize();

	return S_OK;
}
#pragma endregion Initializations

#pragma region
LRESULT CALLBACK OverlordGame::WindowsProcedureStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_CREATE)
	{
		CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)pCS->lpCreateParams);
	}
	else
	{
		OverlordGame* pThisGame = (OverlordGame*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
		if(pThisGame) return pThisGame->WindowsProcedure(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT OverlordGame::WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	if (WindowProcedureHook(hWnd, message, wParam, lParam) == 0)
		return 0;

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT OverlordGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return -1;
}
#pragma endregion Windows Procedures

#pragma region
void OverlordGame::GameLoop()
{
	//Clear Backbuffer
	//m_pDeviceContext->ClearRenderTargetView(m_pCurrentRenderTarget->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::CornflowerBlue));
	m_pDeviceContext->ClearRenderTargetView(m_pCurrentRenderTarget->GetRenderTargetView(), reinterpret_cast<const float*>(&XMFLOAT4(0.12f,0.12f,0.12f,1)));
	m_pDeviceContext->ClearDepthStencilView(m_pCurrentRenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//******
	//UPDATE
	SceneManager::GetInstance()->Update();

	//****
	//DRAW
	SceneManager::GetInstance()->Draw();

	//Present Backbuffer
	m_pSwapchain->Present(0,0);
}

void OverlordGame::SetRenderTarget(RenderTarget* renderTarget)
{
	if(renderTarget == nullptr)
		renderTarget = m_pDefaultRenderTarget;

	auto rtView = renderTarget->GetRenderTargetView();
	m_pDeviceContext->OMSetRenderTargets(1, &rtView, renderTarget->GetDepthStencilView());

	m_pCurrentRenderTarget = renderTarget;
}

// Update PP
RenderTarget* OverlordGame::GetRenderTarget()
{
	return m_pCurrentRenderTarget;
}
#pragma endregion METHODS
