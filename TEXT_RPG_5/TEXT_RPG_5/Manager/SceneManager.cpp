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
#include "../Projectile.h"

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
	//테스트
	ObjectPoolManager::GetInstance()->Preload<Spider>(10, 1);
	ObjectPoolManager::GetInstance()->Preload<Dragon>(3, 1);
	ObjectPoolManager::GetInstance()->Preload<Projectile>(10);
	CurrentPlayer = SpawnObject<Player>("player", 100, 10);
	CurrentPlayer->SetPosition(20, 20);
	
	//  for (int i = 1; i <= 3; ++i)
	// {
	// 	Goblin* Gob = SpawnObject<Goblin>(1);
	// 	Gob->SetPosition(i, i);
	// 	Gob->SetPrevPosition(i, i);
	// 	
	// 	Orc* Or = SpawnObject<Orc>(1);
	// 	Or->SetPosition(i * 2, i * 2);
	// 	Or->SetPrevPosition(i * 2, i * 2);
	// 	
	// 	Slime* Sli = SpawnObject<Slime>(1);
	// 	Sli->SetPosition(i * 2 + 1, i * 2 + 1);
	// 	Sli->SetPrevPosition(i * 2 + 1, i * 2 + 1);
	// 	
	// 	//테스트
	// 	Spider* spi = SpawnObject<Spider>(1);
	// 	spi->SetPosition(i * 3, i * 3);
	// 	spi->SetPrevPosition(i * 3, i * 3);
	//  }
	
	
	//테스트
	Dragon* Dra = SpawnObject<Dragon>(1);
	Dra->SetPosition(10, 10);
	Dra->SetPrevPosition(10, 10);
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
