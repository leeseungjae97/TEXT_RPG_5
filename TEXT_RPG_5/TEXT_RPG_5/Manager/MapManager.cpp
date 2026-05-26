#include "MapManager.h"
#include "../Define.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "../Object.h"
#include "../Monster.h"
#include "../Player.h"
#include "../Projectile.h"

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
		
		Map[PrevPos.Y][PrevPos.X] = {ObjectType::Path, NO_ID};

		if (Pos.Y < 1 
			|| Pos.Y >= MAP_MAX_Y - 1 
			|| Pos.X < 1 
			|| Pos.X >= MAP_MAX_X - 1 
			|| Map[Pos.Y][Pos.X].Type == ObjectType::Wall)
		{
			Obj->SetPosition(PrevPos);
			continue;
		}

		if (Monster* monster = dynamic_cast<Monster*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {ObjectType::Monster, monster->GetID()};
		}
		
		else if (Player* player = dynamic_cast<Player*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {ObjectType::Player, player->GetID()};
		}

		else if (Projectile* projectile = dynamic_cast<Projectile*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {ObjectType::Projectile, projectile->GetID()};
		}
	}
}

void MapManager::BeginPlay()
{
	Map.resize(MAP_MAX_Y, vector<Coordinate>(MAP_MAX_X, {ObjectType::Path, NO_ID}));
	
	for (int i = 0 ; i < MAP_MAX_Y; ++i)
	{
		for (int j = 0 ; j < MAP_MAX_X; ++j)
		{
			if (i == 0 || j == 0 || i == MAP_MAX_Y - 1 || j == MAP_MAX_X - 1)
				Map[i][j] = {ObjectType::Wall, NO_ID};
			else 
				Map[i][j] = {ObjectType::Path, NO_ID};
		}
	}
	
	for (int i = 0; i < 30; ++i)
	{
		int ry = rand() % (MAP_MAX_Y - 2) + 1;
		int rx = rand() % (MAP_MAX_X - 2) + 1;
		
		Map[ry][rx] = {ObjectType::Wall, NO_ID};;
	}
}
