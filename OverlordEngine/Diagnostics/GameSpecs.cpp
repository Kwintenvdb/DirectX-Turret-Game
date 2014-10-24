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

#include "GameSpecs.h"
#include "../Base/GameTime.h"
#include "Logger.h"
#include "../Base/GeneralStructs.h"

int GameSpecs::m_FPS = 0;
float GameSpecs::m_FpsElapsed = 0.0f;
int GameSpecs::m_FpsFrames = 0;

GameSpecs::GameSpecs(void)
{
}


GameSpecs::~GameSpecs(void)
{
}

void GameSpecs::Update(const GameContext& context)
{
	++m_FpsFrames;
	m_FpsElapsed += context.pGameTime->GetElapsed();
	if(m_FpsElapsed >= 1.0f)
	{
		m_FPS = m_FpsFrames;
		m_FpsFrames = 0;
		m_FpsElapsed -= 1.0f;

		//wstringstream ss;
		//ss<<"FPS: ";
		//ss<<m_FPS;

		//Logger::LogInfo(ss.str());
	}
}
