#include "SceneManager.h"

#include "ObjectPoolManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Monster.h"

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	Objects.clear();
	CurrentPlayer = nullptr;
}

void SceneManager::Tick(float DeltaTime)
{
	for (int i = 0; i < Objects.size(); ++i)
	{
		if (Objects[i] != nullptr && !Objects[i]->IsDestroy())
		{
			Objects[i]->Tick(DeltaTime);
		}
	}
}

void SceneManager::Destroy()
{
	for (int i = 0; i < Objects.size();)
	{
		if (Objects[i] == nullptr || Objects[i]->IsDestroy())
		{
			RemoveObject(Objects[i]);
			continue;
		}

		++i;
	}
}

void SceneManager::BeginPlay()
{
	ObjectPoolManager::GetInstance()->Preload<Monster>(10);
	CurrentPlayer = SpawnObject<Player>("player", 100, 100);
	for (int i = 0; i < 6; ++i)
	{
		SpawnObject<Monster>();
	}
}

void SceneManager::AddObject(AObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	if (find(Objects.begin(), Objects.end(), object) != Objects.end())
	{
		return;
	}

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

			ObjectPoolManager::GetInstance()->Return(*it);

			it = Objects.erase(it);
			break;
		}
		else
		{
			++it;
		}
	}
}
