#include "SceneManager.h"

#include "ObjectPoolManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Goblin.h"
#include "../Orc.h"
#include "../Slime.h"

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
	ObjectPoolManager::GetInstance()->Preload<Goblin>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Orc>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Slime>(10, 1);
	CurrentPlayer = SpawnObject<Player>("player", 100, 60);
	for (int i = 0; i < 3; ++i)
	{
		SpawnObject<Goblin>(1);
		SpawnObject<Orc>(1);
		SpawnObject<Slime>(1);
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
