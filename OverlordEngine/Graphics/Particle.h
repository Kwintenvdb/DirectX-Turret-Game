//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.121
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once
#include "../Components/ParticleEmitterComponent.h"
struct GameContext;

class Particle
{
public:
	//Constructor, ParticleEmitterSettings (BY REFERENCE!)
	//the settings of the emitter, passing it by reference makes sure
	//that our particle has the latest settings of the emitter
	Particle(const ParticleEmitterSettings& emitterSettings);
	~Particle(void);

	//Update method
	void Update(const GameContext& context);
	//Init, this method will 'reset' our particle
	void Init(XMFLOAT3 initPosition);
	
	//Method to retrieve the ParticleVertex information of our particle
	ParticleVertex GetVertexInfo() { return m_VertexInfo; }
	//Method to retrieve the state of our particle
	bool IsActive() { return m_IsActive; }

private:

	//ParticleVertex information, used by the emitter's vertexbuffer
	ParticleVertex m_VertexInfo;
	//The emitter's settings (by reference)
	const ParticleEmitterSettings& m_EmitterSettings;
	//An active particle is still alive,
	//An inactive particle can be reused (ReInitialized)
	bool m_IsActive;

	//Total energy before going to an inactive state (seconds)
	float m_TotalEnergy;
	//Energy passed since the particle became active
	float m_CurrentEnergy;
	//Percentual grow/scale during the particle's lifetime
	// <0 = shrink, >0 = grow
	float m_SizeGrow;
	//Initial size of the particle
	float m_InitSize;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Particle(const Particle& yRef);									
	Particle& operator=(const Particle& yRef);
};

