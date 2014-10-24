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

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "ControllerComponent.h"
#include "../Diagnostics/Logger.h"
#include "../Scenegraph/GameObject.h"
#include <math.h>

TransformComponent::TransformComponent(void):
	m_Position(XMFLOAT3(0,0,0)), 
	m_Scale(XMFLOAT3(1,1,1)),
	m_Rotation(XMFLOAT4(0,0,0,1)),
	m_WorldPosition(XMFLOAT3(0,0,0)), 
	m_WorldScale(XMFLOAT3(1,1,1)),
	m_WorldRotation(XMFLOAT4(0,0,0,1)),
	m_Forward(XMFLOAT3(0,0,1)),
	m_Up(XMFLOAT3(0,1,0)),
	m_Right(XMFLOAT3(1,0,0))
{
	m_IsTransformChanged = TransformChanged::NONE;
}


TransformComponent::~TransformComponent(void)
{
}

void TransformComponent::Initialize(const GameContext& gameContext)
{
	UpdateTransforms(gameContext);
}

void TransformComponent::Update(const GameContext& gameContext)
{
	UpdateTransforms(gameContext);
}

void TransformComponent::UpdateTransforms(const GameContext& gameContext)
{
	//TODO:CHANGE
	UNREFERENCED_PARAMETER(gameContext);

	auto rigidbody = m_pGameObject->GetComponent<RigidBodyComponent>();
	auto controller = m_pGameObject->GetComponent<ControllerComponent>();

	if (rigidbody && controller)
	{
		Logger::LogError(L"GameObject cannot have RigidBody and Controller!)");
	}

	if (rigidbody != nullptr && m_IsInitialized)
	{
		if(m_IsTransformChanged & TransformChanged::TRANSLATION)
			rigidbody->Translate(m_Position);
		else
			m_Position = rigidbody->GetPosition();

		if(m_IsTransformChanged & TransformChanged::ROTATION)
			rigidbody->Rotate(m_Rotation);
		else
			m_Rotation = rigidbody->GetRotation();
	}
	else if (controller != nullptr && m_IsInitialized)
	{
		if(m_IsTransformChanged & TransformChanged::TRANSLATION)
			controller->Translate(m_Position);
		else
			m_Position = controller->GetPosition();
	}

	

	
	//Calculate World Matrix
	//**********************
	XMVECTOR rot = XMLoadFloat4(&m_Rotation);
	XMMATRIX world = XMMatrixScaling(m_Scale.x,m_Scale.y,m_Scale.z)*
					XMMatrixRotationQuaternion(rot)*
					XMMatrixTranslation(m_Position.x,m_Position.y,m_Position.z);

	auto parent = m_pGameObject->GetParent();
	if(parent)
	{
		auto parentWorld = XMLoadFloat4x4(&parent->GetTransform()->m_World);
		world *= parentWorld;
	}

	XMStoreFloat4x4(&m_World, world);

	//Get World Transform
	XMVECTOR pos, scale;
	if(XMMatrixDecompose(&scale, &rot, &pos, world))
	{
		XMStoreFloat3(&m_WorldPosition, pos);
		XMStoreFloat3(&m_WorldScale, scale);
		XMStoreFloat4(&m_WorldRotation, rot);
	}

	XMMATRIX rotMat = XMMatrixRotationQuaternion(rot);
	XMVECTOR forward = XMVector3TransformCoord(XMVectorSet(0,0,1,0), rotMat);
	XMVECTOR right = XMVector3TransformCoord(XMVectorSet(1,0,0,0), rotMat);
	XMVECTOR up = XMVector3Cross(forward, right);

	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_Right, right);
	XMStoreFloat3(&m_Up, up);

	//
	m_IsTransformChanged = TransformChanged::NONE;
}

void TransformComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void TransformComponent::Translate(float x, float y, float z)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void TransformComponent::Translate(const XMFLOAT3& position)
{
	Translate(position.x, position.y, position.z);
}

void TransformComponent::Translate(const XMVECTOR& position)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	XMStoreFloat3(&m_Position, position);
}

void TransformComponent::Rotate(float x, float y, float z, bool isEuler)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if(isEuler)
	{
		//x = XMConvertToRadians(x);
		//y = XMConvertToRadians(y);
		//z = XMConvertToRadians(z);

		//float c1 = cos(x / 2);
		//float s1 = sin(x / 2);
		//float c2 = cos(y / 2);
		//float s2 = sin(y / 2);
		//float c3 = cos(z / 2);
		//float s3 = sin(z / 2);


		//m_Rotation.w = c1*c2*c3 - s1*s2*s3;
		//m_Rotation.x = s1*c2*c3 + c1*s2*s3;
		//m_Rotation.y = c1*s2*c3 - s1*c2*s3;
		//m_Rotation.z = c1*c2*s3 + s1*s2*c3;

		XMStoreFloat4(&m_Rotation,XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
	}
	else
	{
		XMStoreFloat4(&m_Rotation,XMQuaternionRotationRollPitchYaw(x, y, z));
	}
}

void TransformComponent::Rotate(const XMFLOAT3& rotation, bool isEuler)
{
	Rotate(rotation.x, rotation.y, rotation.z, isEuler);
}

void TransformComponent::Rotate(const XMVECTOR& rotation, bool isQuaternion)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if(isQuaternion)
	{
		XMStoreFloat4(&m_Rotation, rotation);
	}
	else
	{
		Logger::LogFixMe(L"TransformComponent::Rotate(XMVECTOR, non-Quad) > Not Implemented Yet!");
	}
}

//const XMFLOAT3& TransformComponent::GetRotationEuler() const
//{
//	float q0 = m_Rotation.x;
//	float q1 = m_Rotation.y;
//	float q2 = m_Rotation.z;
//	float q3 = m_Rotation.w;
//
//	float powQ0 = pow(q0, 2);
//	float powQ1 = pow(q1, 2);
//	float powQ2 = pow(q2, 2);
//	float powQ3 = pow(q3, 2);
//
//	//float eulerZ = atan((2 * (q0*q1 + q2*q3)) / (1 - 2 * (powQ1 + powQ2)));
//	//float eulerY = asin(2 * (q0*q2 - q3*q1));
//	//float eulerX = atan((2 * (q0*q3 + q1*q2)) / (1 - 2 * (powQ2 + powQ3)));
//
//	float eulerX = atan2(-2 * (q1*q2 - q3*q0), powQ3 - powQ0 - powQ1 + powQ2);
//	float eulerY = asin(2 * (q0*q2 + q3*q1));
//	float eulerZ = atan2(-2 * (q0*q1 - q3*q2), powQ3 + powQ0 - powQ1 - powQ2);
//
//	return XMFLOAT3(XMConvertToDegrees(eulerX), XMConvertToDegrees(eulerY), XMConvertToDegrees(eulerZ));
//}

void TransformComponent::Scale(float x, float y, float z)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::SCALE;
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void TransformComponent::Scale(const XMFLOAT3& scale)
{
	Scale(scale.x, scale.y, scale.z);
}

bool TransformComponent::CheckConstraints()
{
	if(!m_IsInitialized)
		return true;

	auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();
	if (rigidBody != nullptr && rigidBody->IsStatic())
	{
		Logger::LogWarning(L"[TransformComponent] Constraint Broken: GameObject with a static rigid body cannot move!");
		return false;
	}

	return true;
}
