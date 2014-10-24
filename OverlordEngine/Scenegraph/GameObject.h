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
#include "../Base/GeneralStructs.h"
#include <typeinfo>
#include <functional>

class BaseComponent;
class TransformComponent;
class GameScene;

class GameObject
{
public:
	enum TriggerAction
	{
		ENTER,
		LEAVE
	};

	typedef std::function<void (GameObject* triggerobject, GameObject* otherobject, TriggerAction action)> PhysicsCallback;

	GameObject(void);
	virtual ~GameObject(void);

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj);

	void AddComponent(BaseComponent* pComp);
	void RemoveComponent(BaseComponent* pComp);
	void OnTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
	TransformComponent* GetTransform() const { return m_pTransform; }

	GameScene* GetScene();
	GameObject* GetParent() const { return m_pParentObject; }

	void SetName(string name) { m_Name = name; }
	string GetName() { return m_Name; }

	void SetOnTriggerCallBack(PhysicsCallback callback);

#pragma region
	template<class T> bool HasComponent(bool searchChildren = false)
	{
		return GetComponent<T>(searchChildren) != nullptr;
	}

	template<class T> T* GetComponent(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		for(auto *component : m_pComponents)
		{
			if(component && typeid(*component) == ti)
				return (T*)component;
		}

		if(searchChildren)
		{
			for(auto *child : m_pChildren)
			{
				if (child->GetComponent<T>(searchChildren) != nullptr)
					return child->GetComponent<T>(searchChildren);
			}
		}

		return nullptr;
	}

	template<class T> vector<T*> GetComponents(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		vector<T*> components;

		for(auto *component : m_pComponents)
		{
			if(component && typeid(*component) == ti)
				components.push_back((T*)component);
		}

		if(searchChildren)
		{
			for(auto *child : m_pChildren)
			{
				auto childComponents = child->GetComponents<T>(searchChildren);
			
				for(auto *childComp: childComponents)
					components.push_back((T*)comp);
			}
		}

		return components;
	}

	template<class T> T* GetChild()
	{
		const type_info& ti = typeid(T);
		for(auto *child : m_pChildren)
		{
			if(child && typeid(*child) == ti)
				return (T*)child;
		}
		return nullptr;
	}

	template<class T> vector<T*> GetChildren()
	{
		const type_info& ti = typeid(T);
		vector<T*> children;

		for(auto *child : m_pChildren)
		{
			if(child && typeid(*child) == ti)
				children.push_back((T*)child);
		}
		return children;
	}
#pragma endregion Template Methods

protected:

	virtual void Initialize(const GameContext& gameContext){ UNREFERENCED_PARAMETER(gameContext); };
	virtual void PostInitialize(const GameContext& gameContext){ UNREFERENCED_PARAMETER(gameContext); };
	virtual void Draw(const GameContext& gameContext){ UNREFERENCED_PARAMETER(gameContext); };
	virtual void PostDraw(const GameContext& gameContext){ UNREFERENCED_PARAMETER(gameContext); };
	virtual void Update(const GameContext& gameContext){ UNREFERENCED_PARAMETER(gameContext); };

	string m_Name;

private:
	
	friend class GameScene;

 	void RootInitialize(const GameContext& gameContext);
	void RootUpdate(const GameContext& gameContext);
	void RootDraw(const GameContext& gameContext);
	void RootPostDraw(const GameContext& gameContext);

	vector<GameObject*> m_pChildren;
	vector<BaseComponent*> m_pComponents;

	bool m_IsInitialized, m_IsActive;
	GameScene* m_pParentScene;
	GameObject* m_pParentObject;
	TransformComponent* m_pTransform;
	PhysicsCallback m_OnTriggerCallback;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameObject( const GameObject &obj);
	GameObject& operator=( const GameObject& obj);
};

