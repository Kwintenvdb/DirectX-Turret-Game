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
#include "stdafx.h"
#include "TextureDataLoader.h"
#include <algorithm>


TextureDataLoader::TextureDataLoader(void)
{
}


TextureDataLoader::~TextureDataLoader(void)
{
}

TextureData* TextureDataLoader::LoadContent(const wstring& assetFile)
{
	ID3D11Resource* pTexture;
	ID3D11ShaderResourceView* pShaderResourceView;

	TexMetadata info;
	ScratchImage *image = new ScratchImage();

	//Find Extension
	wstring extension;
	if (assetFile.find_last_of(L".") != wstring::npos)
	{
		extension = assetFile.substr(assetFile.find_last_of(L".") + 1);
	}
	else
	{
		Logger::LogFormat(LogLevel::Error, L"TextureDataLoader::LoadContent() > Invalid File Extensions!\nPath: %s",assetFile.c_str());
		return nullptr;
	}
		
	HRESULT hr;
	if (lstrcmpiW(extension.c_str(), L"dds") == 0) //DDS Loader
	{
		hr = LoadFromDDSFile(assetFile.c_str(), DDS_FLAGS_NONE, &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromDDsFile Failed!"))
			return nullptr;
	}
	else if (lstrcmpiW(extension.c_str(), L"tga") == 0) //TGA Loader
	{
		hr = LoadFromTGAFile(assetFile.c_str(), &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromTGAFile Failed!"))
			return nullptr;
	}
	else //WIC Loader
	{
		hr = LoadFromWICFile(assetFile.c_str(), WIC_FLAGS_NONE, &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromWICFile Failed!"))
			return nullptr;
	}
	

	hr = CreateTexture(m_pDevice, image->GetImages(), image->GetImageCount(),image->GetMetadata(), &pTexture);
	if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > CreateTexture Failed!"))
	{
		SafeDelete(image);
		return nullptr;
	}

	hr = CreateShaderResourceView(m_pDevice, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pShaderResourceView);
	if(Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > CreateShaderResourceView Failed!"))
	{
		SafeDelete(image);
		return nullptr;
	}

	
	SafeDelete(image);
	return new TextureData(pTexture, pShaderResourceView);
}

void TextureDataLoader::Destroy(TextureData* objToDestroy)
{
	SafeDelete(objToDestroy);
}
