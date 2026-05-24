#include "MapManager.h"
#include "../Define.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "../Object.h"
#include "../Monster.h"
#include "../Player.h"

MapManager::MapManager()
{
	map<Vector, Vector> m;
}

MapManager::~MapManager()
{

}

void MapManager::Tick(float DeltaTime)
{
	UpdateMap();
	RenderManager::GetInstance()->AddRender(2,1, Map);
}

void MapManager::UpdateMap()
{
	vector<AObject*> Objects = SceneManager::GetInstance()->GetObjects();

	for (AObject* Obj : Objects)
	{
		if (nullptr == Obj || Obj->IsDestroy())
			continue;
		
		Vector PrevPos = Obj->GetPrevPosition();
		Vector Pos = Obj->GetPosition();
		
		Map[PrevPos.Y][PrevPos.X] = 1;

		if (Pos.Y < 1 || Pos.Y >= MAP_MAX_Y - 1 || Pos.X < 1 || Pos.X >= MAP_MAX_X - 1 || Map[Pos.Y][Pos.X] == 0)
		{
			Obj->SetPosition(PrevPos);
			continue;
		}

		if (Monster* monster = dynamic_cast<Monster*>(Obj))
		{
			Map[Pos.Y][Pos.X] = 4;
		}
		
		else if (Player* player = dynamic_cast<Player*>(Obj))
		{
			Map[Pos.Y][Pos.X] = 3;
		}
	}
}

void MapManager::BeginPlay()
{
	Map.resize(MAP_MAX_Y, vector<int>(MAP_MAX_X, 1));
	//MapOrigin.resize(MAP_MAX_X, vector<int>(MAP_MAX_Y, 1));
	
	for (int i = 0 ; i < MAP_MAX_Y; ++i)
	{
		for (int j = 0 ; j < MAP_MAX_X; ++j)
		{
			if (i == 0 || j == 0 || i == MAP_MAX_Y - 1 || j == MAP_MAX_X - 1)
				Map[i][j] = 0;
			else 
				Map[i][j] = 1;
		}
	}
	
	for (int i = 0; i < 30; ++i)
	{
		int ry = rand() % (MAP_MAX_Y - 2) + 1;
		int rx = rand() % (MAP_MAX_X - 2) + 1;
		
		Map[ry][rx] = 0;
	}
}
