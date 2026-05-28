#include "MapManager.h"
#include "../Define.h"
#include "DisplayManager.h"
#include "SceneManager.h"
#include "ShopManager.h"
#include "StageManager.h"
#include "../Object.h"
#include "../Monster.h"
#include "../Goblin.h"
#include "../Orc.h"
#include "../OrcMage.h"
#include "../Slime.h"
#include "../KingSlime.h"
#include "../Dragon.h"
#include "../Spider.h"
#include "../Mimic.h"
#include "../Player.h"
#include "../Projectile.h"
#include "../Map/Map.h"
#include "../Enum/Boss.h"

MapManager::MapManager()
{
	
}

MapManager::~MapManager()
{
	
}

void MapManager::Tick(float DeltaTime)
{
	
}

// . : Path
// # : Wall
// P : Player 시작 위치
// L : Slime
// O : Orc
// G : Goblin
// D : Spider
// S : Shop
// T : Monster 스폰 위치
// C : Cinema -> 보스 연출이 나오는 트리거 위치
// B : Boss {킹슬라임, 오크메이지, 킹스파이더, 드래곤}
void MapManager::MapParsing(int CurrentStage)
{
	for (int i = 0 ; i < MAP_MAX_Y; ++i)
	{
		for (int j = 0 ; j < MAP_MAX_X; ++j)
		{
			char ch = StringMap[CurrentStage][i][j];
			Vector CurrentPos = {j, i};
			switch (ch)
			{
			case '.': // Path
				{
					Map[i][j] = {MapObjectType::Path, NO_ID};
				}
				break;
			case '#': // Wall
				{
					Map[i][j] = {MapObjectType::Wall, NO_ID};
				}
				break;
			case 'P': // Player
				{
					PlayerPosition = CurrentPos;
				}
				break;
			case 'L': // Slime
				{
					Slime* S = SceneManager::GetInstance()->SpawnObject<Slime>(CurrentStage + 1);
					S->ConfigureForStage(CurrentStage);
					
					S->SetPosition(CurrentPos);
					S->SetPrevPosition(CurrentPos);
					S->SetNextPosition(CurrentPos);
				}
				break;
			case 'O': // Orc
				{
					Orc* O = SceneManager::GetInstance()->SpawnObject<Orc>(CurrentStage + 1);
					O->ConfigureForStage(CurrentStage);
					
					O->SetPosition(CurrentPos);
					O->SetPrevPosition(CurrentPos);
					O->SetNextPosition(CurrentPos);
				}
				break;
			case 'G': // Goblin
				{
					Goblin* G = SceneManager::GetInstance()->SpawnObject<Goblin>(CurrentStage + 1);
					G->ConfigureForStage(CurrentStage);
					
					G->SetPosition(CurrentPos);
					G->SetPrevPosition(CurrentPos);
					G->SetNextPosition(CurrentPos);
				}
				break;
			case 'D': // Spider
				{
					Spider* S = SceneManager::GetInstance()->SpawnObject<Spider>(CurrentStage + 1);
					S->ConfigureForStage(CurrentStage);
					
					S->SetPosition(CurrentPos);
					S->SetPrevPosition(CurrentPos);
					S->SetNextPosition(CurrentPos);
				}
				break;
			case 'S': // Shop
				{
					Map[CurrentPos.Y][CurrentPos.X] = {MapObjectType::Shop, NO_ID};
				}
				break;
			case 'T': // Mimmic or Box
				{
					
				}
				break;
			case 'C': // 나중에
				{
					
				}
				break;
			case 'B':
				{
					BossType Boss = static_cast<BossType>(CurrentStage);
					switch (Boss)
					{
					case BossType::KING_SLIME:
						{
							KingSlime* boss = SceneManager::GetInstance()->SpawnObject<KingSlime>(CurrentStage + 2);
							boss->ConfigureForStage(CurrentStage + 2, true);
							boss->SetPosition(CurrentPos);
							boss->SetPrevPosition(CurrentPos);
							boss->SetNextPosition(CurrentPos);
						}
						break;
					case BossType::ORC_MAGE:
						{
							OrcMage* boss = SceneManager::GetInstance()->SpawnObject<OrcMage>(CurrentStage + 2);
							boss->ConfigureForStage(CurrentStage + 2, true);
							boss->SetPosition(CurrentPos);
							boss->SetPrevPosition(CurrentPos);
							boss->SetNextPosition(CurrentPos);
						}
						break;
					case BossType::KING_SPIDER:
						{
							Dragon* boss = SceneManager::GetInstance()->SpawnObject<Dragon>(CurrentStage + 2);
							boss->ConfigureForStage(CurrentStage + 2, true);
							boss->SetPosition(CurrentPos);
							boss->SetPrevPosition(CurrentPos);
							boss->SetNextPosition(CurrentPos);
						}
						break;
					case BossType::DRAGON:
						{
							Dragon* boss = SceneManager::GetInstance()->SpawnObject<Dragon>(CurrentStage + 2);
							boss->ConfigureForStage(CurrentStage + 2, true);
							boss->SetPosition(CurrentPos);
							boss->SetPrevPosition(CurrentPos);
							boss->SetNextPosition(CurrentPos);
						}
						break;
					}
				}
				break;
			}
		}
	}
}

bool MapManager::IsValidPosition(Vector Pos) const
{
	return Pos.Y >= 1
		&& Pos.Y < MAP_MAX_Y - 1
		&& Pos.X >= 1
		&& Pos.X < MAP_MAX_X - 1;
}

Coordinate MapManager::MakeCoordinate(AObject* Object) const
{
	if (Monster* monster = dynamic_cast<Monster*>(Object))
	{
		return {MapObjectType::Monster, monster->GetID()};
	}

	if (Player* player = dynamic_cast<Player*>(Object))
	{
		return {MapObjectType::Player, player->GetID()};
	}

	if (Projectile* projectile = dynamic_cast<Projectile*>(Object))
	{
		return {MapObjectType::Projectile, projectile->GetID()};
	}

	return {MapObjectType::Path, NO_ID};
}

bool MapManager::CanPlaceObject(AObject* Object, Vector Pos) const
{
	if (!IsValidPosition(Pos))
	{
		return false;
	}

	const Coordinate& Current = Map[Pos.Y][Pos.X];
	if (Current.Type == MapObjectType::Wall || Current.Type == MapObjectType::Shop || Current.Type == MapObjectType::Crystal)
	{
		return false;
	}

	if (Current.ID != NO_ID && Current.ID != Object->GetID())
	{
		return false;
	}

	return true;
}

bool MapManager::MoveObject(AObject* Object, Vector From, Vector To)
{
	if (Object == nullptr || Object->IsDestroy() || !IsMapInitSize())
	{
		return false;
	}

	if (!CanPlaceObject(Object, To))
	{
		return false;
	}

	if (From.Y >= 0 && From.Y < MAP_MAX_Y && From.X >= 0 && From.X < MAP_MAX_X)
	{
		if (Map[From.Y][From.X].ID == Object->GetID())
		{
			Map[From.Y][From.X] = {MapObjectType::Path, NO_ID};
		}
	}

	Map[To.Y][To.X] = MakeCoordinate(Object);
	return true;
}

void MapManager::ClearObject(AObject* Object)
{
	if (Object == nullptr || !IsMapInitSize())
	{
		return;
	}

	Vector Pos = Object->GetPosition();
	if (Pos.Y < 0 || Pos.Y >= MAP_MAX_Y || Pos.X < 0 || Pos.X >= MAP_MAX_X)
	{
		return;
	}

	if (Map[Pos.Y][Pos.X].ID == Object->GetID())
	{
		Map[Pos.Y][Pos.X] = {MapObjectType::Path, NO_ID};
	}
}

void MapManager::BeginPlay()
{
	Map.clear();
	Map.resize(MAP_MAX_Y, vector<Coordinate>(MAP_MAX_X, {MapObjectType::Path, NO_ID}));
	// vector<Vector>& ShopPoses = ShopManager::GetInstance()->GetShopPoses();

	MapParsing(StageManager::GetInstance()->GetCurrentStage());
	
	
	// for (int i = 0 ; i < MAP_MAX_Y; ++i)
	// {
	// 	for (int j = 0 ; j < MAP_MAX_X; ++j)
	// 	{
	// 		if (i == 0 || j == 0 || i == MAP_MAX_Y - 1 || j == MAP_MAX_X - 1)
	// 			Map[i][j] = {MapObjectType::Wall, NO_ID};
	// 		else 
	// 			Map[i][j] = {MapObjectType::Path, NO_ID};
	// 	}
	// }
	//
	// for (int i = 0 ; i < ShopPoses.size(); ++i)
	// {
	// 	Vector Pos = ShopPoses[i];
	// 	Map[Pos.Y][Pos.X] = {MapObjectType::Shop, NO_ID};
	// }
	//
	// const int stage = StageManager::GetInstance()->GetCurrentStage();
	// if (stage == 1)
	// {
	// 	for (int y = 8; y < MAP_MAX_Y - 8; ++y)
	// 	{
	// 		if (y % 3 != 0)
	// 		{
	// 			Map[y][MAP_MAX_X / 2] = { MapObjectType::Wall, NO_ID };
	// 		}
	// 	}
	// 	for (int x = 10; x < MAP_MAX_X - 10; ++x)
	// 	{
	// 		if (x % 4 != 0)
	// 		{
	// 			Map[MAP_MAX_Y / 2][x] = { MapObjectType::Wall, NO_ID };
	// 		}
	// 	}
	// }
	// else if (stage == 2)
	// {
	// 	for (int y = 5; y < MAP_MAX_Y - 5; ++y)
	// 	{
	// 		int leftX = 8 + (y % 8);
	// 		int rightX = MAP_MAX_X - 9 - (y % 8);
	// 		Map[y][leftX] = { MapObjectType::Wall, NO_ID };
	// 		Map[y][rightX] = { MapObjectType::Wall, NO_ID };
	// 	}
	// 	for (int x = 16; x < MAP_MAX_X - 16; ++x)
	// 	{
	// 		if (x % 5 != 0)
	// 		{
	// 			Map[10][x] = { MapObjectType::Wall, NO_ID };
	// 			Map[MAP_MAX_Y - 11][x] = { MapObjectType::Wall, NO_ID };
	// 		}
	// 	}
	// }
	
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
	
	Map[Y][X] = InCoordinate;
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
