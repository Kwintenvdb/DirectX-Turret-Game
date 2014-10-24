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
#include <vector>
#include <typeinfo>
#include "ContentLoader.h"

using namespace std;

class ContentManager
{
public:
	static void Initialize(ID3D11Device* pDevice);
	static void AddLoader(BaseLoader* loader);

	template<class T> 
	static T* Load(const wstring& assetFile)
	{
		const type_info& ti = typeid(T);
		for(BaseLoader* loader:m_Loaders)
		{
			const type_info& loadertype = loader->GetType();
			if(loadertype == ti)
			{
				return (static_cast<ContentLoader<T>*>(loader))->GetContent(assetFile);
			}
		}

		return nullptr;
	}

	static void Release();

private:
	ContentManager();
	~ContentManager(void);

	static vector<BaseLoader*> m_Loaders;
	static ID3D11Device* m_pDevice;
	static bool m_IsInitialized;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ContentManager(const ContentManager& t);
	ContentManager& operator=(const ContentManager& t);
};

