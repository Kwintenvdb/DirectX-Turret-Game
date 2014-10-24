//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "CameraComponent.h"
#include "../Base/OverlordGame.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"
#include "../Physx/PhysxManager.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"

CameraComponent::CameraComponent(void):
	m_FOV(XM_PIDIV4),
	m_NearPlane(0.1f),
	m_FarPlane(2500.0f),
	m_Size(25.0f),
	m_PerspectiveProjection(true),
	m_IsActive(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}


CameraComponent::~CameraComponent(void)
{
}

void CameraComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto windowSettings = OverlordGame::GetGameSettings().Window;
	XMMATRIX projection, view, viewInv, viewProjectionInv;

	if(m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio ,m_NearPlane, m_FarPlane);
	}
	else
	{
		float viewWidth = (m_Size>0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		float viewHeight = (m_Size>0) ? m_Size : windowSettings.Height;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	viewInv = XMMatrixInverse(nullptr, view);
	viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if(gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if(gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext) const
{
	// Step 1: get the GameObject, GameScene and PhysXProxy
	//		   be sure to include safety checks!!
	PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();

	GameScene* scene = m_pGameObject->GetScene();

	if(scene == nullptr)
	{
		Logger::LogError(L"CameraComponent::Pick: No scene to pick from");
		return nullptr;
	}


	if(physX == nullptr)
	{
		Logger::LogError(L"CameraComponent::Pick: Can't get PxPhysx from PhysxManager");
		return nullptr;
	}
	 
	
	// Step 2: created variable declaration for
	//			* near point and far point (float3)
	//			* NDC (float2)
	//			* ray start and ray direction (PxVec3)
	//			* halfwidth and halfheight (Hint: OverlordGame::GetGameSettings holds usefull data)
	//			* mouse position (the game context holds usefull data)
	
	int halfWidth = OverlordGame::GetGameSettings().Window.Width / 2;
	int halfHeight = OverlordGame::GetGameSettings().Window.Height / 2;
	XMFLOAT2 MouseNDC = XMFLOAT2((gameContext.pInput->GetMousePosition().x - halfWidth) / halfWidth,
						(halfHeight - gameContext.pInput->GetMousePosition().y) / halfHeight);


	XMVECTOR nearPoint = XMVectorSet(MouseNDC.x,MouseNDC.y,0.0f,0.0f);
	XMVECTOR farPoint = XMVectorSet(MouseNDC.x,MouseNDC.y,1.0f,0.0f);
	
	XMMATRIX inverse = XMLoadFloat4x4(&m_ViewProjectionInverse);

	nearPoint = XMVector3TransformCoord(nearPoint,inverse);
	farPoint = XMVector3TransformCoord(farPoint,inverse);

	XMFLOAT3 nearPointF3,farPointF3;

	XMStoreFloat3(&nearPointF3,nearPoint);
	XMStoreFloat3(&farPointF3,farPoint);

	PxVec3 rayDirection = PxVec3(farPointF3.x-nearPointF3.x,farPointF3.y-nearPointF3.y,farPointF3.z-nearPointF3.z);
	PxVec3 rayOrigin = PxVec3(nearPointF3.x,nearPointF3.y,nearPointF3.z);
	rayDirection.normalize();

	//rayOrigin=XMVector3TransformCoord(rayOrigin,m_ViewInverse);
	//XMFLOAT4( MouseNDC.x, MouseNDC.y, 0, 0) m_ViewProjectionInverse
	//XMFLOAT3 farPoint = * m_ViewInverse;

	// Step 3: Calculate x and y ndc coordinates

	// Step 4: Calculate near point and far point using the NDC coordinates and the inverse of the viewProjection matrix

	// Step 5: Calculate the ray start and the direction (hint: the direction can be calculated using near point and far point)
	
	// Step 6: Use the PhysxProxy to do a raycast. 
	//		   If the raycast has a hit, return the GameObject associated with the actor
	//		   (Hint: the actor can be fount in hit.block. The userData field of the actor should be a Component)

	PxRaycastBuffer hit;
	PxVec3 pickDirection;
	if(scene->GetPhysxProxy()->Raycast(rayOrigin, rayDirection, PX_MAX_F32, hit))
	{
		BaseComponent* component = static_cast<BaseComponent*>(hit.block.actor->userData);
		return (component->GetGameObject());
	}
	return nullptr;
}

XMFLOAT3 CameraComponent::RaycastPos(const GameContext& gameContext)
{
	PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	GameScene* scene = m_pGameObject->GetScene();

	if(scene == nullptr)
	{
		Logger::LogError(L"CameraComponent::Pick: No scene to pick from");
	}

	if(physX == nullptr)
	{
		Logger::LogError(L"CameraComponent::Pick: Can't get PxPhysx from PhysxManager");
	}
	 
	int halfWidth = OverlordGame::GetGameSettings().Window.Width / 2;
	int halfHeight = OverlordGame::GetGameSettings().Window.Height / 2;
	XMFLOAT2 MouseNDC = XMFLOAT2((gameContext.pInput->GetMousePosition().x - halfWidth) / halfWidth,
						(halfHeight - gameContext.pInput->GetMousePosition().y) / halfHeight);

	XMVECTOR nearPoint = XMVectorSet(MouseNDC.x,MouseNDC.y,0.0f,0.0f);
	XMVECTOR farPoint = XMVectorSet(MouseNDC.x,MouseNDC.y,1.0f,0.0f);	
	XMMATRIX inverse = XMLoadFloat4x4(&m_ViewProjectionInverse);

	nearPoint = XMVector3TransformCoord(nearPoint,inverse);
	farPoint = XMVector3TransformCoord(farPoint,inverse);

	XMFLOAT3 nearPointF3,farPointF3;

	XMStoreFloat3(&nearPointF3,nearPoint);
	XMStoreFloat3(&farPointF3,farPoint);

	PxVec3 rayDirection = PxVec3(farPointF3.x-nearPointF3.x,farPointF3.y-nearPointF3.y,farPointF3.z-nearPointF3.z);
	PxVec3 rayOrigin = PxVec3(nearPointF3.x,nearPointF3.y,nearPointF3.z);
	
	rayDirection.normalize();
	rayOrigin += rayDirection * 10;

	PxRaycastBuffer hit;
	PxVec3 pos;
	if(scene->GetPhysxProxy()->Raycast(rayOrigin, rayDirection, PX_MAX_F32, hit))
	{
		BaseComponent* component = static_cast<BaseComponent*>(hit.block.actor->userData);
		auto go = component->GetGameObject();
		cout << "GO name: " << go->GetName() << endl;
		pos = hit.block.position;
		cout << "RO pos: " << nearPointF3.x << ", " << nearPointF3.y << ", " << nearPointF3.z << endl;
		//cout << "Raycast pos: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
	}

	return XMFLOAT3(pos.x, pos.y, pos.z);
}