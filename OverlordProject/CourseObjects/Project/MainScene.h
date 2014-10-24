#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"
#include "Graphics\SpriteFont.h"

class Enemy;
class Player;
class GameObject;
class Fire;
class Turret;
class Projectile;
class LevelPiece;
class SpawnPoint;
class Pickup;
class BlurMaterial;
class GrayMaterial;
class ResourceBar;
class SkinnedDiffuseMaterial;

class MainScene: public GameScene
{
public:
	MainScene(void);
	virtual ~MainScene(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	void CreateInputs(const GameContext& gameContext);
	void CreateMaterials(const GameContext& gameContext);
	LevelPiece* CreateLevelPiece(float width, float height, float depth, XMFLOAT3 pos = XMFLOAT3(0,0,0), XMFLOAT4 color = (XMFLOAT4)Colors::White);
	void CreateTerrain(const GameContext& gameContext);
	void CreateTurrets(const GameContext& gameContext);
	void CreatePickups();
	void CreateSpawnPoints();
	void CreateSpawnPoint(float x, float y, float z);

	void AddProjectile(Projectile* proj);

	void SpawnEnemies(const GameContext& gameContext);
	void ManagePlayer(const GameContext& gameContext);
	void ManageHUD(const GameContext& gameContext);
	void ManageTurrets(const GameContext& gameContext);
	void ManageProjectiles(const GameContext& gameContext);
	void ManageEnemies(const GameContext& gameContext);
	void ManagePickups(const GameContext& gameContext);

	template<class T> void RemoveFromVec(T* ptr, vector<T*>& vec);

	GameObject* m_Terrain, *m_HP;
	Player* m_Player;
	vector<Enemy*>		m_Enemies;
	Enemy* m_pEnemy;
	vector<Fire*>		m_Fires;
	vector<Turret*>		m_Turrets;
	vector<Projectile*> m_Projectiles;
	vector<SpawnPoint*> m_SpawnPoints;
	vector<Pickup*>		m_Pickups;
	vector<GameObject*> m_TurretSprites;

	SpriteFont* m_pSpriteFont;

	float m_Timer;
	bool m_Debug;

	BlurMaterial* m_pBlurMat;
	GrayMaterial* m_pGrayMat;
	bool m_MatRemoved;

	ResourceBar* m_HpBar, *m_SpeedBar, *m_InvBar;
	GameObject* m_DeadScreen, *m_WinScreen;
	bool m_Won;

	float m_BlurIt;
	SkinnedDiffuseMaterial* m_pMatPlayer;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MainScene( const MainScene &obj);
	MainScene& operator=( const MainScene& obj);
};