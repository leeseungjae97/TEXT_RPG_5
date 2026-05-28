#include "SceneManager.h"

#include "ObjectPoolManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Goblin.h"
#include "../Orc.h"
#include "../Slime.h"
#include "../Spider.h"
#include "../Dragon.h"
#include "../KingSlime.h"
#include "../OrcMage.h"
#include"../Mimic.h"
#include "../Projectile.h"
#include "MapManager.h"
#include "StageManager.h"

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
	EnsurePools();
	if (CurrentPlayer == nullptr)
	{
		CurrentPlayer = SpawnObject<Player>("player", 100, 30);
	}

	LoadCurrentStage();
}

void SceneManager::EnsurePools()
{
	if (bPoolsInitialized)
	{
		return;
	}

	ObjectPoolManager::GetInstance()->Preload<Goblin>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Orc>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Slime>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Spider>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<KingSlime>(1, 1);
	ObjectPoolManager::GetInstance()->Preload<OrcMage>(1, 1);
	ObjectPoolManager::GetInstance()->Preload<Mimic>(1, 1);
	ObjectPoolManager::GetInstance()->Preload<Dragon>(3, 1);
	ObjectPoolManager::GetInstance()->Preload<Projectile>(10);
	bPoolsInitialized = true;
}

void SceneManager::ClearStageObjects()
{
	for (int i = 0; i < static_cast<int>(Objects.size());)
	{
		if (Objects[i] == nullptr || Objects[i] == CurrentPlayer)
		{
			++i;
			continue;
		}

		ObjectPoolManager::GetInstance()->Return(Objects[i]);
		Objects.erase(Objects.begin() + i);
	}
}

void SceneManager::LoadCurrentStage()
{
	EnsurePools();
	ClearStageObjects();

	if (CurrentPlayer == nullptr)
	{
		CurrentPlayer = SpawnObject<Player>("player", 100, 30);
	}
	const int stage = StageManager::GetInstance()->GetCurrentStage();
	MapManager::GetInstance()->MapParsing(stage);
	
	Vector playerStart = MapManager::GetInstance()->GetPlayerPosition(); 
	CurrentPlayer->SetPosition(playerStart);
	CurrentPlayer->SetPrevPosition(playerStart);
	CurrentPlayer->SetNextPosition(playerStart);
}

void SceneManager::Reset()
{
	Objects.clear();
	ObjectPoolManager::GetInstance()->ClearAll();
	CurrentPlayer = nullptr;
	bPoolsInitialized = false;
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
