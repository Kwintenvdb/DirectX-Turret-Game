#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "../Helpers/BinaryReader.h"
#include "../Content/ContentManager.h"
#include "../Graphics/TextRenderer.h"


SpriteFontLoader::SpriteFontLoader()
{

}


SpriteFontLoader::~SpriteFontLoader()
{
}

SpriteFont* SpriteFontLoader::LoadContent(const wstring& assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if (!binReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout
	
	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...

	if(binReader->Read<byte>() != 66 || binReader->Read<byte>() != 77 || binReader->Read<byte>() != 70)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}
	


	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...
	if(binReader->Read<byte>() < 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [SpriteFont::m_FontName]
	//...

	byte blockID = binReader->Read<byte>();
	int blockSize = binReader->Read<int>();
	pSpriteFont->m_FontSize = binReader->Read<short>();
	binReader->MoveBufferPosition(12);
	pSpriteFont->m_FontName = binReader->ReadNullString();


	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	//...

	blockID = binReader->Read<byte>();
	blockSize = binReader->Read<int>();
	binReader->MoveBufferPosition(4);
	pSpriteFont->m_TextureHeight = binReader->Read<short>();
	pSpriteFont->m_TextureWidth = binReader->Read<short>();
	//binReader->MoveBufferPosition(4);
	int pages = binReader->Read<short>();

	if(pages > 1)
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");

	binReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	//...

	blockID = binReader->Read<char>();
	blockSize = binReader->Read<int>();

	std::wstring pageName = binReader->ReadNullString();

	if(pageName.empty())
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");

	std::wstring textureFilePath = assetFile.substr(0, assetFile.rfind('/')+1);
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(textureFilePath + pageName);
	
	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
	//...
	
	blockID = binReader->Read<char>();
	blockSize = binReader->Read<int>();

	int charCount = blockSize / 20;

	for(int x = 0; x < charCount; x++)
	{
		wchar_t characterID = wchar_t(binReader->Read<int>());

		if(!pSpriteFont->IsCharValid( characterID ))
		{
			Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid character");
			binReader->MoveBufferPosition( 16 );
			continue;
		}

		FontMetric &metric = pSpriteFont->GetMetric(characterID);
		metric.IsValid = true;
		metric.Character = characterID;

		float xPos = binReader->Read<short>();
		float yPos = binReader->Read<short>();

		metric.Width = binReader->Read<short>();
		metric.Height = binReader->Read<short>();

		metric.OffsetX = binReader->Read<short>();
		metric.OffsetY = binReader->Read<short>();
		metric.AdvanceX = binReader->Read<short>();
		metric.Page = binReader->Read<byte>();
		//metric.Channel = binReader->Read<byte>();

		switch(binReader->Read<byte>())
		{
		case 1:
			metric.Channel = 2;
			break;

		case 2: 
			metric.Channel = 1;
			break;

		case 4:
			metric.Channel = 0;
			break;
		}

		
		metric.TexCoord.x = xPos / float(pSpriteFont->m_TextureWidth);
		metric.TexCoord.y = yPos / float(pSpriteFont->m_TextureHeight);
		
		
	}
	
	//DONE :)

	delete binReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
