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
#include "../Helpers/Singleton.h"

class PhysxAllocator;
class PhysxErrorCallback;
class GameScene;
class OverlordGame;

class PhysxManager: public Singleton<PhysxManager>, public PxVisualDebuggerConnectionHandler
{
public:

	friend class Singleton<PhysxManager>;

	//GETTERS
	PxPhysics* GetPhysics() const { return m_pPhysics; }
	
	//Methods
	PxScene* CreateScene(GameScene* pScene);

	//PVD
	virtual void onPvdSendClassDescriptions( PxVisualDebuggerConnection& connection );
	virtual void onPvdConnected(PxVisualDebuggerConnection& connection);
	virtual void onPvdDisconnected(PxVisualDebuggerConnection& connection);

	bool ToggleVisualDebuggerConnection();

private:

	friend class OverlordGame;

	PhysxManager(void);
	virtual ~PhysxManager(void);

	void Init(void* pDevice);
	void CleanUp();

	PhysxAllocator* m_pDefaultAllocator;
	PhysxErrorCallback* m_pDefaultErrorCallback;
	PxFoundation* m_pFoundation;
	PxProfileZoneManager* m_pProfileZoneManager;
	PxPhysics* m_pPhysics;
	PxDefaultCpuDispatcher* m_pDefaultCpuDispatcher;
	PxCudaContextManager* m_pCudaContextManager;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PhysxManager( const PhysxManager &obj);
	PhysxManager& operator=( const PhysxManager& obj );
};

