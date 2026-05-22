#include "SceneManager.h"
#include "Object.h"
#include "Player.h"
#include "Monster.h"


SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	for (int i = 0; i < Objects.size(); ++i)
	{
		if (nullptr != Objects[i])
		{
			Objects[i]->Destroy();
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

void SceneManager::Init()
{
	Player* player = new Player("name", 10, 10);
	AddObject(player);

	Monster* monster = new Monster();
	AddObject(monster);
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
			Objects.erase(it);
			delete (*it);
			break;
		}
	}
}
