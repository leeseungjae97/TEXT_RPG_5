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
		CurrentPlayer = SpawnObject<Player>("player", 100, 200);
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
		CurrentPlayer = SpawnObject<Player>("player", 100, 200);
	}

	Vector playerStart = StageManager::GetInstance()->GetPlayerStartPosition();
	CurrentPlayer->SetPosition(playerStart);
	CurrentPlayer->SetPrevPosition(playerStart);
	CurrentPlayer->SetNextPosition(playerStart);
	
	const int stage = StageManager::GetInstance()->GetCurrentStage();
	const int monsterLevel = stage + 1;
	const int offset = stage * 5;
	for (int i = 1; i <= 3; ++i)
	{
		Goblin* Gob = SpawnObject<Goblin>(monsterLevel);
		Gob->ConfigureForStage(monsterLevel);
		Gob->SetPosition(i + offset, i + offset);
		Gob->SetPrevPosition(i + offset, i + offset);
		
		Orc* Or = SpawnObject<Orc>(monsterLevel);
		Or->ConfigureForStage(monsterLevel);
		Or->SetPosition(i * 2 + offset, i * 2 + offset);
		Or->SetPrevPosition(i * 2 + offset, i * 2 + offset);
		
		Slime* Sli = SpawnObject<Slime>(monsterLevel);
		Sli->ConfigureForStage(monsterLevel);
		Sli->SetPosition(i * 2 + 1 + offset, i * 2 + 1 + offset);
		Sli->SetPrevPosition(i * 2 + 1 + offset, i * 2 + 1 + offset);
		
		Spider* spi = SpawnObject<Spider>(monsterLevel);
		spi->ConfigureForStage(monsterLevel);
		spi->SetPosition(i * 3 + offset, i * 3 + offset);
		spi->SetPrevPosition(i * 3 + offset, i * 3 + offset);
	}
  //보스테스트
	Vector bossPosition = { min(MAP_MAX_X - 4, 28 + offset), min(MAP_MAX_Y - 4, 14 + offset) };
	
	if (stage == 0)
	{
		KingSlime* boss = SpawnObject<KingSlime>(monsterLevel + 2);
		boss->ConfigureForStage(monsterLevel + 2, true);
		boss->SetPosition(bossPosition);
		boss->SetPrevPosition(bossPosition);
	}
	else if (stage == 1)
	{
		Dragon* boss = SpawnObject<Dragon>(monsterLevel + 2);
		boss->ConfigureForStage(monsterLevel + 2, true);
		boss->SetPosition(bossPosition);
		boss->SetPrevPosition(bossPosition);
	}
	else
	{
		Dragon* boss = SpawnObject<Dragon>(monsterLevel + 2);
		boss->ConfigureForStage(monsterLevel + 2, true);
		boss->SetPosition(bossPosition);
		boss->SetPrevPosition(bossPosition);
	}
	
	// Dragon* boss = SpawnObject<Dragon>(monsterLevel + 2);
	// boss->ConfigureForStage(monsterLevel + 2, true);
	// Vector bossPosition = { min(MAP_MAX_X - 4, 28 + offset), min(MAP_MAX_Y - 4, 14 + offset) };
	// boss->SetPosition(bossPosition);
	// boss->SetPrevPosition(bossPosition);
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
