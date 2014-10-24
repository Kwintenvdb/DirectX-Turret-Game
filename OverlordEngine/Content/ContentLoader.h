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
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "../Diagnostics/Logger.h"

class BaseLoader
{
public:
	BaseLoader(void){}
	virtual ~BaseLoader(void){}

	virtual const type_info& GetType() const = 0;
	virtual void Unload() = 0;
	virtual void SetDevice(ID3D11Device* pDevice) { m_pDevice = pDevice; }

protected:
	ID3D11Device* m_pDevice;

private:
	BaseLoader( const BaseLoader &obj);
	BaseLoader& operator=( const BaseLoader& obj );
};

template <class T>
class ContentLoader : public BaseLoader
{
public:
	ContentLoader(void)
	{
		++m_loaderReferences;
	}

	~ContentLoader(void)
	{
		
	}

	virtual const type_info& GetType() const { return typeid(T); }

	T* GetContent(const wstring& assetFile)
	{
		for(pair<wstring, T*> kvp:m_contentReferences)
		{
			if(kvp.first.compare(assetFile)==0)
				return kvp.second;
		}

		//Does File Exists?
		struct _stat buff;
		int result = -1;
		result = _wstat(assetFile.c_str(), &buff);
		if(result != 0)
		{
			wstringstream ss;
			ss<<"ContentManager> File not found!\nPath: ";
			ss<<assetFile;
			Logger::LogError(ss.str());
		}


		T* content = LoadContent(assetFile);
		if(content!=nullptr)m_contentReferences.insert(pair<wstring,T*>(assetFile, content));

		return content;
	}

	virtual void Unload()
	{
		--m_loaderReferences;

		if(m_loaderReferences<=0)
		{
			for(pair<wstring,T*> kvp:m_contentReferences)
			{
				Destroy(kvp.second);
			}

			m_contentReferences.clear();
		}
	}

protected:
	virtual T* LoadContent(const wstring& assetFile) = 0;
	virtual void Destroy(T* objToDestroy) = 0;

private:
	static unordered_map<wstring, T*> m_contentReferences;
	static int m_loaderReferences;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ContentLoader( const ContentLoader &obj);
	ContentLoader& operator=( const ContentLoader& obj );
};

template<class T>
unordered_map<wstring, T*> ContentLoader<T>::m_contentReferences = unordered_map<wstring, T*>();

template<class T>
int ContentLoader<T>::m_loaderReferences = 0;

