#include "MapManager.h"
#include "../Define.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "ShopManager.h"
#include "../Object.h"
#include "../Monster.h"
#include "../Player.h"
#include "../Projectile.h"

MapManager::MapManager()
{
	
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
		
		Map[PrevPos.Y][PrevPos.X] = {MapObjectType::Path, NO_ID};

		if (Pos.Y < 0
			|| Pos.Y >= MAP_MAX_Y 
			|| Pos.X < 0
			|| Pos.X >= MAP_MAX_X 
			|| Map[Pos.Y][Pos.X].Type == MapObjectType::Wall
			|| Map[Pos.Y][Pos.X].Type == MapObjectType::Shop)
		{
			// Obj->SetPosition(PrevPos);
			continue;
		}

		if (Monster* monster = dynamic_cast<Monster*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {MapObjectType::Monster, monster->GetID()};
		}
		
		else if (Player* player = dynamic_cast<Player*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {MapObjectType::Player, player->GetID()};
		}

		else if (Projectile* projectile = dynamic_cast<Projectile*>(Obj))
		{
			Map[Pos.Y][Pos.X] = {MapObjectType::Projectile, projectile->GetID()};
		}
	}
}

void MapManager::BeginPlay()
{
	Map.resize(MAP_MAX_Y, vector<Coordinate>(MAP_MAX_X, {MapObjectType::Path, NO_ID}));
	vector<Vector>& ShopPoses = ShopManager::GetInstance()->GetShopPoses();
	
	for (int i = 0 ; i < MAP_MAX_Y; ++i)
	{
		for (int j = 0 ; j < MAP_MAX_X; ++j)
		{
			if (i == 0 || j == 0 || i == MAP_MAX_Y - 1 || j == MAP_MAX_X - 1)
				Map[i][j] = {MapObjectType::Wall, NO_ID};
			else 
				Map[i][j] = {MapObjectType::Path, NO_ID};
		}
	}
	
	for (int i = 0 ; i < ShopPoses.size(); ++i)
	{
		Vector Pos = ShopPoses[i];
		Map[Pos.Y][Pos.X] = {MapObjectType::Shop, NO_ID};
	}
	
	// for (int i = 0; i < 30; ++i)
	// {
	// 	int ry = rand() % (MAP_MAX_Y - 2) + 1;
	// 	int rx = rand() % (MAP_MAX_X - 2) + 1;
	// 	
	// 	Map[ry][rx] = {MapObjectType::Wall, NO_ID};;
	// }
}

bool MapManager::IsTypeExist(Vector Pos, MapObjectType Type)
{
	if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		return false;
	
	if (Map[Pos.Y][Pos.X].Type == Type)
	{
		return true;
	}
	
	return false;
}
bool MapManager::IsTypeExist(int Y, int X, MapObjectType Type)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return false;
	
	if (Map[Y][X].Type == Type)
	{
		return true;
	}
	return false;	
}

int MapManager::GetID(int Y, int X)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return NO_ID;
	
	return Map[Y][X].ID;
}

int MapManager::GetID(Vector Pos)
{
	if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		return NO_ID;
	
	return Map[Pos.Y][Pos.X].ID;
}

void MapManager::SetMapObjectCoordinate(int Y, int X, Coordinate InCoordinate)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return;
	
	Map[Y][X] = {MapObjectType::Path, NO_ID};
}

void MapManager::SetMapObjectType(int Y, int X, MapObjectType Type)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return;
	
	Map[Y][X].Type = Type;
}

void MapManager::SetMapObjectType(Vector Pos, MapObjectType Type)
{
	if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		return;
	
	Map[Pos.Y][Pos.X].Type = Type;
}

void MapManager::SetMapObjectID(int Y, int X, int InID)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return;
	
	Map[Y][X].ID = InID;
}
MapObjectType MapManager::GetType(Vector Pos)
{
	if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		return MapObjectType::None;
	
	return Map[Pos.Y][Pos.X].Type;
}
MapObjectType MapManager::GetType(int Y, int X)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return MapObjectType::None;
	
	return Map[Y][X].Type;
}

AObject* MapManager::GetMapObject(int Y, int X, MapObjectType Type)
{
	if (Y < 0 || Y > MAP_MAX_Y - 1 || X < 0 || X > MAP_MAX_X - 1)
		return nullptr;
	
	if (IsTypeExist(Y, X,  Type))
	{
		int ObjectID = GetID(Y, X);
		return ObjectPoolManager::GetInstance()->GetObjectByID(ObjectID);
	}
	return nullptr;
}

AObject* MapManager::GetMapObject(Vector Pos, MapObjectType Type)
{
	if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		return nullptr;
	
	if (IsTypeExist(Pos,  Type))
	{
		int ObjectID = GetID(Pos);
		return ObjectPoolManager::GetInstance()->GetObjectByID(ObjectID);
	}
	return nullptr;
}

bool MapManager::IsMapInitSize()
{
	if (Map.empty() || Map[0].empty())
		return false;
	return true;
}
