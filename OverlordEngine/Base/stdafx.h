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

#pragma region
//PhysX:you must suppress its definition of the macros min and max, as these are common C++ method names in the SDK
#define NOMINMAX
// Windows Header Files:
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "XINPUT9_1_0.LIB")

//C RunTime Header Files
#include <wchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;
#pragma endregion Windows

#pragma region
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//DirectXMath
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
using namespace DirectX;
#pragma endregion DirectX

#pragma region
#include <PxPhysicsAPI.h>
using namespace physx;

#if defined(DEBUG) || defined(_DEBUG)
	#pragma comment(lib, "PxTaskDEBUG.lib")
	#pragma comment(lib, "PhysX3DEBUG_x86.lib")
	#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
	#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
	#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
	#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
	#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")

#else 
	#pragma comment(lib, "PxTask.lib")
	#pragma comment(lib, "PhysX3_x86.lib")
	#pragma comment(lib, "PhysX3Common_x86.lib")
	#pragma comment(lib, "PhysX3Extensions.lib")
	#pragma comment(lib, "PhysXProfileSDK.lib")
	#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
	#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#endif
#pragma endregion PHYSX INCLUDES

#pragma region
//**EFFECTS11 (Helper for loading Effects (D3DX11))
//https://fx11.codeplex.com/
//MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
//MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
#include "../../AdditionalLibraries/DX_Effects11/include/d3dx11effect.h"
#if _MSC_VER == 1800
#if defined(DEBUG) || defined(_DEBUG)
	#pragma comment(lib, "../AdditionalLibraries/DX_Effects11/lib/DxEffects11_d_VS13.lib")
	#pragma comment(lib, "dxguid.lib")
#else 
	#pragma comment(lib, "../AdditionalLibraries/DX_Effects11/lib/DxEffects11_VS13.lib")
#endif
#elif _MSC_VER == 1700
#if defined(DEBUG) || defined(_DEBUG)
	#pragma comment(lib, "../AdditionalLibraries/DX_Effects11/lib/DxEffects11_d_VS12.lib")
	#pragma comment(lib, "dxguid.lib")
#else 
	#pragma comment(lib, "../AdditionalLibraries/DX_Effects11/lib/DxEffects11_VS12.lib")
#endif
#endif

//*DXTEX (Helper for loading Textures (D3DX11))
//http://directxtex.codeplex.com/
//MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
//MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
#include "../../AdditionalLibraries/DX_Tex/include/DirectXTex.h"
#if _MSC_VER == 1800
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "../AdditionalLibraries/DX_Tex/lib/VS13/Debug_/DirectXTex.lib")
#else 
#pragma comment(lib, "../AdditionalLibraries/DX_Tex/lib/VS13/Release_/DirectXTex.lib")
#endif
#elif _MSC_VER == 1700
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "../AdditionalLibraries/DX_Tex/lib/VS12/Debug_/DirectXTex.lib")
#else 
#pragma comment(lib, "../AdditionalLibraries/DX_Tex/lib/VS12/Release_/DirectXTex.lib")
#endif
#endif
#pragma endregion Additional Libraries

#pragma region
//*****************************************************************************
//Declare templates for releasing interfaces and deleting objects 
//*****************************************************************************
template<class Interface>
inline void SafeRelease(Interface &pInterfaceToRelease)
{
    if (pInterfaceToRelease != 0)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = 0;
    }
}

template<class T>
inline void SafeDelete(T &pObjectToDelete)
{
	if (pObjectToDelete != 0)
    {
		delete(pObjectToDelete);
		pObjectToDelete = 0;
	}
}

template<typename T>
inline void Clamp(T& value, T hi, T lo)
{
	if(value > hi)
		value = hi;

	if(value < lo)
		value = lo;
}
#pragma endregion Templates & Macros

#include "GeneralStructs.h"
