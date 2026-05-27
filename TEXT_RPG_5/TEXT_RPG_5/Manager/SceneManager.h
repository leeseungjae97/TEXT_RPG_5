#pragma once
#include "../Singleton.h"
#include "../pch.h"
#include "../Object.h"
#include "ObjectPoolManager.h"

class AObject;
class Player;
class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Destroy();
	void Reset();
	void LoadCurrentStage();

public:
	void AddObject(AObject* object);
	void RemoveObject(AObject* object);
	vector<AObject*>& GetObjects() { return Objects; }
	Player* GetPlayer() const { return CurrentPlayer; }


	template<typename T, typename... Args>
	T* SpawnObject(Args&&... args)
	{
		static_assert(is_base_of<AObject, T>::value, "T must derive from AObject");

		T* Object = ObjectPoolManager::GetInstance()->Get<T>(std::forward<Args>(args)...);
		AddObject(Object);
		Object->BeginPlay();

		return Object;
	}

private:
	void EnsurePools();
	void ClearStageObjects();
	vector<AObject*> Objects;
	Player* CurrentPlayer = nullptr;
	bool bPoolsInitialized = false;
};
