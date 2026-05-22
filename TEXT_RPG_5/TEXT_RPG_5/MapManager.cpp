#include "MapManager.h"
#include "Define.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Object.h"
#include "Monster.h"

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
	RenderManager::GetInstance()->AddRender(3, 0, Map);
}

void MapManager::UpdateMap()
{
	vector<AObject*> Objects = SceneManager::GetInstance()->GetObjects();

	for (AObject* Obj : Objects)
	{
		if (nullptr == Obj || Obj->IsDestroy())
			continue;
		Vector PrevPos = Obj->GetPrevPosition();
		
		Map[PrevPos.Y][PrevPos.X] = 1;

		Vector Pos = Obj->GetPosition();

		if (Pos.Y < 0 || Pos.Y > MAP_MAX_Y - 1 || Pos.X < 0 || Pos.X > MAP_MAX_X - 1)
		{
			Obj->SetPosition(PrevPos);
			continue;
		}

		if (Monster* monster = dynamic_cast<Monster*>(Obj))
		{
			Map[Pos.Y][Pos.X] = 4;
		}
		else
			Map[Pos.Y][Pos.X] = 3;
		
	}
}

void MapManager::Init()
{
	// MAP_MAX_X * MAP_MAX_Y ¸Ê ¸¸µé±â
	Map.resize(MAP_MAX_Y, vector<int>(MAP_MAX_X, 1));
	//MapOrigin.resize(MAP_MAX_X, vector<int>(MAP_MAX_Y, 1));
	
	Map[0] = { 0,0,0,0,0,0,0, };
	Map[1] = { 0,1,1,1,1,1,0, };
	Map[2] = { 0,1,1,1,1,1,0, };
	Map[3] = { 0,1,1,1,1,1,0, };
	Map[4] = { 0,1,1,1,1,1,0, };
	Map[5] = { 0,1,1,1,1,1,0, };
	Map[6] = { 0,1,1,1,1,1,0, };
}
