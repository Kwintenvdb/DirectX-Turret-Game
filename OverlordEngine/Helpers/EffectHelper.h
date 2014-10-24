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
#include <map>

//Input Layout Description
enum ILSemantic : UINT
{
	NONE = 0,
	POSITION = (1 << 0),
	NORMAL = (1 << 1),
	COLOR = (1 << 2),
	TEXCOORD = (1 << 3),
	BINORMAL = (1 << 4),
	TANGENT = (1 << 5),
	BLENDINDICES = (1 << 6),
	BLENDWEIGHTS = (1 << 7)
};

struct ILDescription
{
	ILSemantic SemanticType;
	DXGI_FORMAT Format;
	UINT SemanticIndex;
	UINT Offset;
};

class EffectHelper final
{
public:
	EffectHelper()
	{ 
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::POSITION, L"POSITION") ); 
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::NORMAL, L"NORMAL") );
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::COLOR, L"COLOR") ); 
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::TEXCOORD, L"TEXCOORD") ); 
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BINORMAL, L"BINORMAL") ); 
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::TANGENT, L"TANGENT") );
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BLENDINDICES, L"BLENDINDICES") );
		//m_IlSemanticName.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BLENDWEIGHTS, L"BLENDWEIGHTS") );
	};
	~EffectHelper(){};

public:
	static bool BuildInputLayout(ID3D11Device* pDevice, ID3DX11EffectTechnique* pTechnique, ID3D11InputLayout **pInputLayout, vector<ILDescription>& inputLayoutDescriptions, int& inputLayoutSize, UINT& inputLayoutID);
	static bool BuildInputLayout(ID3D11Device* pDevice, ID3DX11EffectTechnique* pTechnique, ID3D11InputLayout **pInputLayout, UINT& inputLayoutSize);
	static const wstring& GetIlSemanticName(ILSemantic semantic);

private:
	static std::map<ILSemantic, wstring> create_map()
	{
		std::map<ILSemantic, wstring> m;
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::POSITION, L"POSITION") ); 
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::NORMAL, L"NORMAL") );
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::COLOR, L"COLOR") ); 
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::TEXCOORD, L"TEXCOORD") ); 
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BINORMAL, L"BINORMAL") ); 
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::TANGENT, L"TANGENT") );
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BLENDINDICES, L"BLENDINDICES") );
		m.insert ( std::pair<ILSemantic,wstring>(ILSemantic::BLENDWEIGHTS, L"BLENDWEIGHTS") );
		return m;
	}
	static std::map<ILSemantic, wstring> m_IlSemanticName;
	static const wstring m_empty;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	EffectHelper(const EffectHelper &obj);
	EffectHelper& operator=(const EffectHelper& obj);
};


