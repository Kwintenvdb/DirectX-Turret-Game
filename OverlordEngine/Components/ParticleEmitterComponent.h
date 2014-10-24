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
#include "BaseComponent.h"


struct ParticleEmitterSettings
{
public:

	ParticleEmitterSettings():
		MinSize(0.1f),
		MaxSize(2.0f),
		MinEnergy(1.0f),
		MaxEnergy(2.0f),
		Velocity(XMFLOAT3(0, 0, 0)),
		MaxEmitterRange(10.0f),
		MinEmitterRange(9.0f),
		MinSizeGrow(1.0f),
		MaxSizeGrow(1.0f),
		RandomRot(true),
		Color((XMFLOAT4) Colors::White){}

	//The minimum size each particle can be at the time when it is spawned
	float MinSize;
	//The maximum size each particle can be at the time when it is spawned
	float MaxSize;
	//The minimum lifetime of each particle, measured in seconds
	float MinEnergy;
	//The maximum lifetime of each particle, measured in seconds
	float MaxEnergy;
	//The starting speed of particles along X, Y and Z, measured in the object's orientation
	XMFLOAT3 Velocity;
	//The maximum radius that the particles are spawned in
	float MaxEmitterRange;
	//The minimum radius that the particles are spawned in
	float MinEmitterRange;
	//The percentual maximum change in size during the particle's lifetime
	float MinSizeGrow;
	//The percentual minimum change in size during the particle's lifetime
	float MaxSizeGrow;
	//The color of a particle
	XMFLOAT4 Color;
	//Use random rotation
	bool RandomRot;
};

struct ParticleVertex
{
public: 

	ParticleVertex(XMFLOAT3 pos = XMFLOAT3(0, 0, 0), XMFLOAT4 col = (XMFLOAT4)Colors::White, float size = 5.0f, float rotation = 0) :
		Position(pos),
		Color(col),
		Size(size),
		Rotation(rotation){}

	XMFLOAT3 Position;
	XMFLOAT4 Color;
	float Size;
	float Rotation;
};

class Particle;
class TextureData;

class ParticleEmitterComponent: public BaseComponent
{
public:
	//Constructor, particle texture path and max particles
	ParticleEmitterComponent(const wstring& assetFile, int particleCount = 50);
	~ParticleEmitterComponent(void);

	//EmitterSettings SETTERS
	void SetMinSize(float minSize) { m_Settings.MinSize = minSize; }
	void SetMaxSize(float maxSize) { m_Settings.MaxSize = maxSize; }
	void SetMinEnergy(float minEnergy) { m_Settings.MinEnergy = minEnergy; }
	void SetMaxEnergy(float maxEnergy) { m_Settings.MaxEnergy = maxEnergy; }
	void SetVelocity(XMFLOAT3 velocity) { m_Settings.Velocity = velocity; }
	void SetMinSizeGrow(float minSizeGrow) { m_Settings.MinSizeGrow = minSizeGrow; }
	void SetMaxSizeGrow(float maxSizeGrow) { m_Settings.MaxSizeGrow = maxSizeGrow; }
	void SetMinEmitterRange(float minEmitterRange) { m_Settings.MinEmitterRange = minEmitterRange; }
	void SetMaxEmitterRange(float maxEmitterRange) { m_Settings.MaxEmitterRange = maxEmitterRange; }
	void SetColor(XMFLOAT4 color) { m_Settings.Color = color; }
	void SetRandomRot(bool rot) { m_Settings.RandomRot = rot; }

	int GetActiveParticles();
	void SetActive(bool active);
	
protected:
	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);
	virtual void PostDraw(const GameContext& gameContext) override;

private:

	//Method to load effect-related stuff
	void LoadEffect(const GameContext& gameContext);
	//Method to create the vertex buffer
	void CreateVertexBuffer(const GameContext& gameContext);

	//The actual vertex buffer, containing ParticleVertex information for each Particle
	ID3D11Buffer* m_pVertexBuffer;

	//The effect we'll use to render the particles
	ID3DX11Effect *m_pEffect;
	//The default technique
	ID3DX11EffectTechnique *m_pDefaultTechnique;
	//Shader matrix variables
	ID3DX11EffectMatrixVariable *m_pWvpVariable, *m_pViewInverseVariable;
	//Shader texture variable
	ID3DX11EffectShaderResourceVariable *m_pTextureVariable;
	//ShaderResourceView, containing the particle texture
	TextureData *m_pParticleTexture;
	
	//Vertex input layout variables
	ID3D11InputLayout *m_pInputLayout;
	UINT m_pInputLayoutSize;

	//Vector of particle pointers
	vector<Particle*> m_Particles;
	//The settings for this particle system
	ParticleEmitterSettings m_Settings;
	//The total amount of particles (m_Particles.size() == m_ParticleCount)
	int m_ParticleCount;
	//The active particles for a certain frame
	int m_ActiveParticles;
	//Total seconds since the last particle initialisation
	float m_LastParticleInit;
	//string containing the path to the particle texture
	wstring m_AssetFile;

	bool m_IsActive;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ParticleEmitterComponent(const ParticleEmitterComponent& yRef);									
	ParticleEmitterComponent& operator=(const ParticleEmitterComponent& yRef);
};

