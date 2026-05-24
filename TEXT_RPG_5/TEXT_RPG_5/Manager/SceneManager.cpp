#include "SceneManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Monster.h"

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	for (int i = 0; i < Objects.size(); ++i)
	{
		if (nullptr != Objects[i])
		{
			delete Objects[i];
		}
	}
	Objects.clear();
}

void SceneManager::Tick(float DeltaTime)
{
	for (int i = 0; i < Objects.size(); ++i)
	{
		Objects[i]->Tick(DeltaTime);
	}
}

void SceneManager::Destroy()
{
	for (int i = 0; i < Objects.size(); ++i)
	{
		if (Objects[i]->IsDestroy())
		{
			RemoveObject(Objects[i]);
		}
	}
}

void SceneManager::BeginPlay()
{
	CurrentPlayer = SpawnObject<Player>("name", 10, 10);
	SpawnObject<Monster>();
	SpawnObject<Monster>();
	SpawnObject<Monster>();
	SpawnObject<Monster>();
	SpawnObject<Monster>();
	SpawnObject<Monster>();
}

void SceneManager::AddObject(AObject* object)
{
	Objects.push_back(object);
}

void SceneManager::RemoveObject(AObject* object)
{
	auto it = Objects.begin();
	while (it != Objects.end())
	{
		if ((*it) == object)
		{
			if ((*it) == CurrentPlayer)
			{
				CurrentPlayer = nullptr;
			}

			delete (*it);
			it = Objects.erase(it);
			break;
		}
		else
		{
			++it;
		}
	}
}
