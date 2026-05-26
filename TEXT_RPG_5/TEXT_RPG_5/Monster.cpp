#include "Monster.h"
#include "Manager/SceneManager.h"
#include "Manager/MapManager.h"
#include "Player.h"
#include "Define.h"
#include <cstdlib>
#include <cmath>

Monster::Monster()
{
	Name = "Monster";
	Health = 100;
	MaxHealth = Health;
	Attack = 10;
	
	Position = Vector(4, 4);
	PrevPosition = Position;
	
	MoveElapsedtime = 0.0f;
	
	MoveInterval = 0.7f;
	
	DetectionRange = 4;
	
	bUseBfs = true;
}

Monster::~Monster()
{

}

string Monster::GetName()
{
	return Name;
}

int Monster::GetHealth()
{
	return Health;
}

int Monster::GetMaxHealth()
{
	return MaxHealth;
}

int Monster::GetAttack()
{
	return Attack;
}

float Monster::GetMoveAlpha() const
{
	if (MoveInterval <= 0.0f)
	{
		return 1.0f;
	}

	return min(MoveElapsedtime / MoveInterval, 1.0f);
}

void Monster::TakeDamage(int Damage)
{
	Health -= Damage;

	if (Health < 0)
	{
		Health = 0;
	}
}

bool Monster::IsDead()
{
	return Health <= 0;
}

void Monster::Tick(float DeltaTime)
{
	AObject::Tick(DeltaTime);
	if (IsDead())
	{
		Destroy();
		return;
	}

	MoveElapsedtime += DeltaTime;


	if (MoveElapsedtime < MoveInterval)
	{
		return;
	}

	MoveElapsedtime = 0.0f;


	Player* player = FindPlayer();


	if (player == nullptr)
	{
		MoveRandom();
		return;
	}

	int Distance = GetDistanceToPlayer(player);

	if (Distance <= DetectionRange)
	{
		if (bUseBfs)
		{
			MoveTowardPlayerBfs();
		}
		else
		{
			MoveTowardPlayer(player);
		}
	}
	else
	{
		MoveRandom();
	}
}

void Monster::Destroy()
{
	bIsDestroy = true;
}

Player* Monster::FindPlayer()
{
	
	return SceneManager::GetInstance()->GetPlayer();
	
	//vector<AObject*>& objects = SceneManager::GetInstance()->GetObjects();

	//for (int i = 0; i < objects.size(); ++i)
	//{
		//AObject* obj = objects[i];

		//if (obj == nullptr)
		//{
			//continue;
		//}
	
	// 	if (obj->IsDestroy())
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	Player* player = dynamic_cast<Player*>(obj);
	//
	// 	if (player != nullptr)
	// 	{
	// 		return player;
	// 	}
	// }
	//
	// return nullptr;
}

int Monster::GetDistanceToPlayer(Player* player)
{
	if (player == nullptr)
	{
		return 9999;
	}
	
	Vector PlayerPos = player->GetPosition();
	
	
	int DistanceX = abs(PlayerPos.X - Position.X);
	
	int DistanceY = abs(PlayerPos.Y - Position.Y);

	return DistanceX + DistanceY;
}

void Monster::MoveTowardPlayer(Player* player)
{
	if (player == nullptr)
	{
		return;
	}

	
	PrevPosition = Position;
	
	Vector PlayerPos = player->GetPosition();
	
	int DiffX = PlayerPos.X - Position.X;
	int DiffY = PlayerPos.Y - Position.Y;

	
	if (abs(DiffX) > abs(DiffY))
	{
		if (DiffX > 0)
		{
			Position.X++;
		}
		else if (DiffX < 0)
		{
			Position.X--;
		}
	}
	
	else
	{
		if (DiffY > 0)
		{
			Position.Y++;
		}
		else if (DiffY < 0)
		{
			Position.Y--;
		}
	}
}

void Monster::MoveRandom()
{
	PrevPosition = Position;
	
	int RandDir = rand() % 4;

	switch (RandDir)
	{
	case 0:
		
		Position.Y--;
		break;

	case 1:
		
		Position.Y++;
		break;

	case 2:
		
		Position.X--;
		break;

	case 3:
		
		Position.X++;
		break;


	}
}

void Monster::MoveTowardPlayerBfs()
{
	Player* player = SceneManager::GetInstance()->GetPlayer();
	
	if (player == nullptr)
	{
		return;
	}
	
	Vector Start = Position;
	Vector Target = player->GetPosition();
	
	vector<vector<Coordinate>>& WorldMap = MapManager::GetInstance()->GetMap();
	
	queue<Vector> Q;
	bool visited[MAP_MAX_Y][MAP_MAX_X] = {};
	Vector Parent[MAP_MAX_Y][MAP_MAX_X];
	
	
	Vector Move[4]
	{
		{0,1},
		{0,-1},
		{1,0},
		{-1,0},
	};
	
	Q.push(Start);
	visited[Start.Y][Start.X] = true;
	
	bool found = false;
	
	while (!Q.empty())
	{
		Vector Current = Q.front();
		Q.pop();
		
		if (Current.X == Target.X && Current.Y == Target.Y)
		{
			found = true;
			break;
		}
		for (int i = 0; i < 4; ++i)
		{
			Vector Go;
			
			Go.X = Current.X + Move[i].X;
			Go.Y = Current.Y + Move[i].Y;
			
			if (Go.X < 0 || Go.X >= MAP_MAX_X || Go.Y < 0 || Go.Y >= MAP_MAX_Y)
			{
				continue;
			}
			
			if (visited[Go.Y][Go.X])
			{
				continue;
			}
			
			if (WorldMap[Go.Y][Go.X].Type == ObjectType::Wall)
			{
				continue;
			}
			if (WorldMap[Go.Y][Go.X].Type == ObjectType::Monster)
			{
				continue;
			}
			
			visited[Go.Y][Go.X] = true;
			Parent[Go.Y][Go.X]= Current;
			Q.push(Go);
		}
	}	
	if (!found)
	{
		MoveRandom();
		return;
	}
	
	vector<Vector> Moving;
	
	Vector Current = Target;
	
	while (!(Current.X == Start.X && Current.Y == Start.Y))
	{
		Moving.push_back(Current);
		
		Vector Before = Parent[Current.Y][Current.X];
		
		if (Before.X == -1 || Before.Y == -1)
		{
			return;
		}
		
		Current = Before;
	}
	
	Moving.push_back(Start);
	
	reverse(Moving.begin(), Moving.end());
	
	if (Moving.size() < 2)
	{
		return;
	}
	
	Vector NextPosition = Moving[1];

	PrevPosition = Position;
	
	Position = NextPosition;
}
		


