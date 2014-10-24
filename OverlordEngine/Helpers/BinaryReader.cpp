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

#include "BinaryReader.h"


BinaryReader::BinaryReader(void):
	m_Exists(false),
	m_pReader(nullptr)
{
}

BinaryReader::~BinaryReader(void)
{
	Close();
}

wstring BinaryReader::ReadLongString()
{
	if(m_pReader==nullptr)
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	auto stringLength = Read<UINT>();
	
	wstringstream ss;
	for(UINT i=0; i<stringLength; ++i)
	{
		ss<<Read<wchar_t>();
	}

	return (wstring)ss.str();
}

wstring BinaryReader::ReadNullString()
{
	if(m_pReader==nullptr)
	{
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");
		return L"";
	}

	string buff;
	getline(*m_pReader, buff, '\0');

	return wstring(buff.begin(),buff.end());
}


wstring BinaryReader::ReadString()
{
	if(m_pReader==nullptr)
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	int stringLength = (int)Read<char>();
	
	wstringstream ss;
	for(int i=0; i<stringLength; ++i)
	{
		ss<<Read<char>();
	}

	return (wstring)ss.str();
}

void BinaryReader::Open(wstring binaryFile)
{
	Close();

	auto temp = new ifstream();
	temp->open(binaryFile, ios::in|ios::binary);
	if(temp->is_open())
	{
		m_pReader = temp;
		m_Exists = true;
	}
	else
	{
		wstringstream ss;
		ss<<L"BinaryReader::Open> Failed to open the file!\nBinaryFile: ";
		ss<<binaryFile;
		Logger::LogWarning(ss.str());
		Close();
	}
}

void BinaryReader::Open(char* s, UINT32 size)
{
	Close();

	string data(s, size);
	m_pReader = new istringstream(data);
	m_Exists = true;
}

void BinaryReader::Close()
{
	SafeDelete(m_pReader);
	m_Exists = false;
}

int BinaryReader::GetBufferPosition()
{
	if(m_pReader)
	{
		return static_cast<int>(m_pReader->tellg());
	}

	Logger::LogWarning(L"BinaryReader::GetBufferPosition> m_pReader doesn't exist");
	return -1;
}

bool BinaryReader::SetBufferPosition(int pos)
{
	if(m_pReader)
	{
		m_pReader->seekg(pos);
		return true;
	}

	Logger::LogWarning(L"BinaryReader::SetBufferPosition> m_pReader doesn't exist");
	return false;
}

bool BinaryReader::MoveBufferPosition(int move)
{
	auto currPos = GetBufferPosition();
	if(currPos>0)
	{
		return SetBufferPosition(currPos + move);
	}

	return false;
}
