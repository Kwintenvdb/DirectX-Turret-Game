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
class GameTime
{
public:
	GameTime(void);
	~GameTime(void);

	void Reset();
	void Update();

	float GetElapsed() const { return m_ElapsedGameTime; }
	float GetTotal() const { return m_TotalGameTime; }
	int GetFPS() const { return m_FPS; }
	void ForceElapsedUpperbound(bool force, float upperBound = 0.03f){m_ForceElapsedUpperBound = force; m_ElapsedUpperBound = upperBound;}
	bool IsRunning() const { return !m_IsStopped; }

	void Start();
	void Stop();

private:

	float m_TotalGameTime;
	float m_ElapsedGameTime;
	float m_SecondsPerCount;

	bool m_ForceElapsedUpperBound;
	float m_ElapsedUpperBound;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_IsStopped;

	int m_FPS;
	float m_FpsTimer;
	int m_FpsCount;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameTime( const GameTime &obj);
	GameTime& operator=( const GameTime& obj);
};

