#include "stdafx.h"
#include "BoneObject.h"
#include "../Components/Components.h"
#include "../Components//ModelComponent.h"

BoneObject::BoneObject(int boneId, int materialId, float length)
	:m_BoneId(boneId)
	,m_MaterialId(materialId)
	,m_Length(length)
{
}

BoneObject::~BoneObject(void)
{
}

void BoneObject::Initialize(const GameContext& gameContext)
{

	UNREFERENCED_PARAMETER(gameContext);

	GameObject* emptyObject = new GameObject();
	ModelComponent* component = new ModelComponent(L"./Resources/Meshes/Bone.ovm");
	component->SetMaterial(m_MaterialId);
	
	emptyObject->AddComponent(component);

	emptyObject->GetTransform()->Scale(m_Length, m_Length, m_Length);
	AddChild(emptyObject);
}

void BoneObject::AddBone(BoneObject* bonePtr)
{
	bonePtr->GetTransform()->Translate(0,0,-m_Length);
	//GetChild<GameObject>()->AddChild(bonePtr);
	AddChild(bonePtr);
}

XMFLOAT4X4 BoneObject::GetBindPose()
{
	return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
	XMMATRIX inverse=XMLoadFloat4x4(&GetTransform()->GetWorld());
	inverse = XMMatrixInverse( nullptr, inverse);
	XMStoreFloat4x4(&m_BindPose, inverse);

	for(auto bone : GetChildren<BoneObject>())
	{
		bone->CalculateBindPose();
	}
}