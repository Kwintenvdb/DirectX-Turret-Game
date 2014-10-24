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

#include "GameTime.h"


GameTime::GameTime(void):
		m_ElapsedGameTime(0.0f),
		m_TotalGameTime(0.0f),
		m_SecondsPerCount(0.0f),
		m_BaseTime(0),
		m_PausedTime(0),
		m_StopTime(0),
		m_PrevTime(0),
		m_CurrTime(0),
		m_IsStopped(true),
		m_FPS(0),
		m_FpsTimer(0.0f),
		m_FpsCount(0),
		m_ElapsedUpperBound(0.03f),
		m_ForceElapsedUpperBound(false)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_SecondsPerCount = 1.0f / (float)countsPerSecond;
}


GameTime::~GameTime(void)
{
}

void GameTime::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_FpsTimer = 0.0f;
	m_FpsCount = 0;
	m_IsStopped = false;
}

void GameTime::Update()
{
		if(m_IsStopped)
	{
		m_FPS = 0;
		m_ElapsedGameTime = 0.0f;
		m_TotalGameTime = (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_BaseTime);
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	m_ElapsedGameTime = (float)((m_CurrTime - m_PrevTime) * m_SecondsPerCount);
	m_PrevTime = m_CurrTime;

	if(m_ElapsedGameTime < 0.0f)
		m_ElapsedGameTime = 0.0f;

	if(m_ForceElapsedUpperBound && m_ElapsedGameTime > m_ElapsedUpperBound)
	{
		m_ElapsedGameTime = m_ElapsedUpperBound;
	}

	m_TotalGameTime = (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);

	//FPS LOGIC
	m_FpsTimer += m_ElapsedGameTime;
	++m_FpsCount;
	if(m_FpsTimer >= 1.0f)
	{
		m_FPS = m_FpsCount;
		m_FpsCount = 0;
		m_FpsTimer -= 1.0f;
	}
}

void GameTime::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(m_IsStopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_IsStopped = false;
	}
}

void GameTime::Stop()
{
	if(!m_IsStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_IsStopped = true;
	}
}
