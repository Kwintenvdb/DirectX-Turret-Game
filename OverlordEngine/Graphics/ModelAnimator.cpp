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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ModelAnimator.h"
#include "../Diagnostics/Logger.h"


ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(vector<XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_PlayOnce(false),
m_Completed(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}


ModelAnimator::~ModelAnimator()
{
	//SafeDelete(m_pMeshFilter);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size

	if(clipNumber > m_pMeshFilter->m_AnimationClips.size())
	{
		Reset();
		Logger::Log(LogLevel::Warning, L"ModelAnimator::SetAnimation(UINT) -- current clip is higher than amount of clips");
		return;
	}
	else
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
	//If not,
	//	Call Reset
	//	Log a warning with an appropriate message
	//	return
	//else
	//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
	//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(wstring clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message

	bool foundClip = false;
	for( AnimationClip clip : m_pMeshFilter->m_AnimationClips )
	{
		if(clip.Name == clipName)
		{
			SetAnimation(clip);
			foundClip = true;
		}
	}

	if(!foundClip)
	{
		Reset();
		Logger::Log(LogLevel::Warning, L"ModelAnimator::SetAnimation(wstring) -- can't find requested clip");
	}

}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;

	Reset(false);	
	//Call Reset(false)
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	m_IsPlaying = !pause;
	
	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	if(m_ClipSet)
	{
		//m_Transforms.assign(m_CurrentClip.Keys[0].BoneTransforms, m_CurrentClip.Keys[0].BoneTransforms);
		std::vector<XMFLOAT4X4> boneTransformers = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(boneTransformers.begin(), boneTransformers.end());
	}
	else
	{
		XMMATRIX identityMatrix = XMMatrixIdentity();
		XMFLOAT4X4 identityMatrix2;
		XMStoreFloat4x4(&identityMatrix2, identityMatrix);
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identityMatrix2);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);
		//passedTicks = passedTicks % m_CurrentClip.Duration;

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if(m_Reversed)
		{
			m_TickCount -= passedTicks;

			if(m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			if(!m_Completed)
				m_TickCount += passedTicks;

			if(m_TickCount > m_CurrentClip.Duration)
			{
				if(m_PlayOnce)
				{
					m_Completed = true;
					m_TickCount = m_CurrentClip.Duration - 5.f;
				}
				else m_TickCount -= m_CurrentClip.Duration;
			}
		}
		
		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		
		for(AnimationKey key : m_CurrentClip.Keys)
		{
			if(m_TickCount > key.Tick)
				keyA = key;

			if(m_TickCount < key.Tick)
			{
				keyB = key;
				break;
			}
		}
		
		//4.
		//Interpolate between keys
		float differenceKeyAKeyB = keyB.Tick - keyA.Tick;
		float blendKeyA = 1-((m_TickCount - keyA.Tick) / differenceKeyAKeyB);
		//Figure out the BlendFactor (See lab document)
		//Clear the m_Transforms vector
		m_Transforms.clear();

		int x = 0;
		for(XMFLOAT4X4 boneTransform : keyA.BoneTransforms)
		{

			auto transformA = keyA.BoneTransforms[x];
			auto transformB = keyB.BoneTransforms[x];

			XMMATRIX transformMatrixA, transformMatrixB;
			transformMatrixA = XMLoadFloat4x4(&transformA);
			transformMatrixB = XMLoadFloat4x4(&transformB);

			XMVECTOR scaleA, translationA, rotationA, scaleB, translationB, rotationB, scaleLerped, translationLerped, rotationLerped;
			XMMatrixDecompose(&scaleA, &rotationA, &translationA, transformMatrixA);
			XMMatrixDecompose(&scaleB, &rotationB, &translationB, transformMatrixB);

			//float lerpControlFactor = blendKeyA ;//* m_AnimationSpeed;// * gameContext.pGameTime->GetElapsed();
			scaleLerped = XMVectorLerp(scaleA, scaleB, blendKeyA);
			translationLerped = XMVectorLerp(translationA, translationB, blendKeyA);
			rotationLerped = XMQuaternionSlerp(rotationA, rotationB, blendKeyA);

			//XMMATRIX transform = XMMATRIX(translationLerped, rotationLerped, scaleLerped, XMVECTOR());
			XMMATRIX transform = XMMatrixIdentity();
			transform *= XMMatrixScalingFromVector(scaleLerped);
			transform *= XMMatrixRotationQuaternion(rotationLerped);
			transform *= XMMatrixTranslationFromVector(translationLerped);
			

			XMFLOAT4X4 finalTransform;
								
			XMStoreFloat4x4(&finalTransform, transform);
			m_Transforms.push_back(finalTransform);

			x++;

		}
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
	}
}
