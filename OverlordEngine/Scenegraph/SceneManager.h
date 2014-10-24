//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.113
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once
#include "../Helpers/Singleton.h"

//Forward Declarations
class GameScene;
class OverlordGame;

class SceneManager final: public Singleton<SceneManager>
{
public:
	~SceneManager(void);

	void AddGameScene(GameScene* scene);
	void RemoveGameScene(GameScene* scene);
	void SetActiveGameScene(wstring sceneName);
	void NextScene();
	void PreviousScene();
	GameScene* GetActiveScene() const { return m_ActiveScene; }
	// Update PP
	OverlordGame* GetGame();

private:

	friend class OverlordGame;
	friend class Singleton<SceneManager>;

	SceneManager(void);	

	// Update PP
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, OverlordGame* game);
	void Update();
	void Draw();

	vector<GameScene*> m_pScenes;
	bool m_IsInitialized;
	GameScene* m_ActiveScene, *m_NewActiveScene;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	OverlordGame* m_pGame;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SceneManager( const SceneManager &obj);
	SceneManager& operator=( const SceneManager& obj);
};

