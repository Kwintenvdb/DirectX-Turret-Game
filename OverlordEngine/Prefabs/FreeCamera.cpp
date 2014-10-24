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

#include "FreeCamera.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"

FreeCamera::FreeCamera(void):
	m_MoveSpeed(10),
	m_RotationSpeed(90.f),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_pCamera(nullptr)
{
	SetName("FreeCamera");
}


FreeCamera::~FreeCamera(void)
{
}

void FreeCamera::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Camera Component
	m_pCamera = new CameraComponent();
	AddComponent(m_pCamera);
}

void FreeCamera::SetRotation(float pitch, float yaw)
{
	m_TotalPitch = pitch;
	m_TotalYaw = yaw;
}

void FreeCamera::Update(const GameContext& gameContext)
{
	if(m_pCamera->IsActive())
	{
		//auto initialRotation = GetTransform()->GetRotationEuler();
		//m_TotalPitch = initialRotation.x;
		//m_TotalYaw = initialRotation.y;

		//HANDLE INPUT
		XMFLOAT2 move = XMFLOAT2(0,0);
		move.y = gameContext.pInput->IsKeyboardKeyDown('W')?1.0f:0.0f;
		if(move.y == 0) move.y = -(gameContext.pInput->IsKeyboardKeyDown('S')?1.0f:0.0f);
		if(move.y == 0) move.y = gameContext.pInput->GetThumbstickPosition().y;

		move.x = gameContext.pInput->IsKeyboardKeyDown('D')?1.0f:0.0f;
		if(move.x == 0) move.x = -(gameContext.pInput->IsKeyboardKeyDown('A')?1.0f:0.0f);
		if(move.x == 0) move.x = gameContext.pInput->GetThumbstickPosition().x;

		XMFLOAT2 look = XMFLOAT2(0,0);
		if(gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			auto mouseMove = gameContext.pInput->GetMouseMovement();
			look.x = static_cast<float>(mouseMove.x);
			look.y = static_cast<float>(mouseMove.y);
		}

		if(look.x == 0 && look.y == 0)
		{
			look = gameContext.pInput->GetThumbstickPosition(false);
		}

		//CALCULATE TRANSFORMS
		XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
		XMVECTOR right = XMLoadFloat3(&GetTransform()->GetRight());
		XMVECTOR currPos = XMLoadFloat3(&GetTransform()->GetPosition());
		//XMVECTOR currRot = XMLoadFloat4(&GetTransform()->GetRotation());
		//XMVECTOR finalQuat;

		currPos += forward * move.y * m_MoveSpeed * gameContext.pGameTime->GetElapsed();
		currPos += right * move.x * m_MoveSpeed * gameContext.pGameTime->GetElapsed();
		
		m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
		m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();

		////MATRIX APPROACH
		//XMMATRIX yawRotationMat, pitchRotationMat, finalRotMat;
		//XMVECTOR transformedRight;
		//yawRotationMat = XMMatrixRotationAxis(XMVectorSet(0,1,0,0), m_TotalYaw);
		//transformedRight = XMVector3TransformCoord(XMVectorSet(1,0,0,0), yawRotationMat);
		//pitchRotationMat = XMMatrixRotationAxis(transformedRight, m_TotalPitch);
		//finalRotMat = yawRotationMat * pitchRotationMat;	
		//finalQuat = XMQuaternionRotationMatrix(finalRotMat);

		////QUAT APPROACH
		//XMVECTOR yawQuat, pitchQuat;
		//yawQuat = XMQuaternionRotationAxis(XMVectorSet(0,1,0,0), m_TotalYaw);
		//pitchQuat = XMQuaternionRotationAxis(XMVectorSet(1,0,0,0), m_TotalPitch);
		//finalQuat = XMQuaternionMultiply(yawQuat, pitchQuat);	
		//finalQuat = XMQuaternionMultiply(finalQuat, currRot);
		
		GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
		GetTransform()->Translate(currPos);
	}
}
