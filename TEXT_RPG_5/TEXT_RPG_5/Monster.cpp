#include "Monster.h"
#include "Manager/SceneManager.h"
#include "Player.h"
#include <cstdlib>
#include <cmath>

#include "Define.h"
#include "Component/LevelComponent.h"
#include "Manager/MapManager.h"

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
	NotifyDamage(Damage);

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
		if (Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer())
		{
			if (LevelComponent* LevelComp = PlayerPtr->GetComponent<LevelComponent>())
			{
				LevelComp->AddExp(50);
			}
		}
		MapManager::GetInstance()->GetMap()[Position.Y][Position.X].Type = ObjectType::Path;
		MapManager::GetInstance()->GetMap()[Position.Y][Position.X].ID = NO_ID;
		
		Destroy();
		return;
	}

	MoveElapsedtime += DeltaTime * GetSlowRatio();


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
		MoveTowardPlayer(player);
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

void Monster::OnSpawnFromPool()
{
	AObject::OnSpawnFromPool();
	Health = MaxHealth;
}

void Monster::OnReturnToPool()
{
	AObject::OnReturnToPool();
}

Player* Monster::FindPlayer()
{
	
	vector<AObject*>& objects = SceneManager::GetInstance()->GetObjects();

	for (int i = 0; i < objects.size(); ++i)
	{
		AObject* obj = objects[i];

		if (obj == nullptr)
		{
			continue;
		}

		
		if (obj->IsDestroy())
		{
			continue;
		}
		
		Player* player = dynamic_cast<Player*>(obj);

		if (player != nullptr)
		{
			return player;
		}
	}
	
	return nullptr;
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


