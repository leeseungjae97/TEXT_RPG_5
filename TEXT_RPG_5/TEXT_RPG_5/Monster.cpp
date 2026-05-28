#include "Monster.h"
#include "Manager/SceneManager.h"
#include "Manager/MapManager.h"
#include "Manager/ItemManager.h"
#include "Manager/ViewportManager.h"
#include "component/InventoryComponent.h"
#include "Item/Item.h"
#include "Player.h"
#include "Define.h"
#include "pch.h"
#include "Define.h"
#include "Component/LevelComponent.h"
#include "Manager/BattleManager.h"
#include "Manager/StageManager.h"

Monster::Monster()
{
	Name = L"Monster";
	Level = 1;
	Health = 100;
	MaxHealth = Health;
	Attack = 10;
	
	//Position = Vector(4, 4);
	PrevPosition = Position;
	
	MoveElapsedtime = 0.0f;
	
	MoveInterval = 0.7f;
	
	DetectionRange = 4;
	
	bUseBfs = true;
	
	AttackRange = 1;
	AttackElapsedtime = 0.0f;
	AttackInterval = 1.5f;
	AttackVisibleTime = 0.0f;
	AttackVisibleDuration = 0.18f;
	AttackTelegraphElapsedTime = 0.0f;
	AttackTelegraphDuration = 0.6f;
	bAttackTelegraphActive = false;
}

Monster::~Monster()
{

}

string Monster::GetName()
{
	if (Name.find(L"고블린") != wstring::npos) return "Goblin";
	
	if (Name.find(L"킹슬라임") != wstring::npos) return "KingSlime";
	if (Name.find(L"슬라임") != wstring::npos) return "Slime";
	
	if (Name.find(L"오크메이지") != wstring::npos) return "OrcMage";
	if (Name.find(L"오크") != wstring::npos) return "Orc";
	
	if (Name.find(L"드래곤") != wstring::npos) return "Dragon";
	if (Name.find(L"거미") != wstring::npos) return "Spider";
	if (Name.find(L"Troll") != wstring::npos) return "Troll";
	if (Name.find(L"미믹") != wstring::npos) return "Mimic";
	
	return "Monster";
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

void Monster::TransitionAttackTelegraphToAttack(float DeltaTime)
{
	if (bAttackTelegraphActive)
	{
		AttackTelegraphElapsedTime += DeltaTime * GetSlowRatio();
		AttackVisibleTime = max(AttackTelegraphDuration - AttackTelegraphElapsedTime, 0.0f);
		if (AttackTelegraphElapsedTime >= AttackTelegraphDuration)
		{
			ExecutePendingAttack(SceneManager::GetInstance()->GetPlayer());
			bAttackTelegraphActive = false;
			AttackTelegraphElapsedTime = 0.0f;
			AttackVisibleTime = AttackVisibleDuration;
		}
	}
	else if (AttackVisibleTime > 0.0f)
	{
		AttackVisibleTime -= DeltaTime;
		if (AttackVisibleTime <= 0.0f)
		{
			AttackVisibleTime = 0.0f;
			AttackValue.clear();
		}
	}
}

void Monster::Tick(float DeltaTime)
{
	AObject::Tick(DeltaTime);

	if (IsDead())
	{
		//부활테스트
		if (Revive())
		{
			return;
		}
		
		Destroy();
		return;
	}

	MoveElapsedtime += DeltaTime * GetSlowRatio();
	AttackElapsedtime += DeltaTime * GetSlowRatio();
	if (IsMoving())
	{
		CommitMoveIfNeeded(GetMoveAlpha());
	}

	TransitionAttackTelegraphToAttack(DeltaTime);
	
	Player* player = SceneManager::GetInstance()->GetPlayer();
	
	if (player == nullptr)
	{
		return;
	}
	
	if (player->IsDead())
	{
		return;
	}

	if (bAttackTelegraphActive)
	{
		return;
	}

	if (MoveElapsedtime >= MoveInterval)
	{
		FinishMoveIfNeeded(GetMoveAlpha());
	}
	
	if (CanAttackplayer(player))
	{
		Attackplayer(player);
		return;
	}
	
	if (MoveElapsedtime < MoveInterval)
	{
		return;
	}

	MoveElapsedtime = 0.0f;
	
	int Distance = GetDistanceToPlayer(player);

	if (Distance <= DetectionRange)
	{
		if (bUseBfs)
		{
			MoveTowardPlayerBfs();
		}
		//else
		//{
			//MoveTowardPlayer(player);
		//}
	}
	else
	{
		MoveRandom();
	}
}

void Monster::Destroy()
{
	BattleManager::GetInstance()->RegisterKilledMonster(GetName());
	if (Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer())
	{
		if (LevelComponent* LevelComp = PlayerPtr->GetComponent<LevelComponent>())
		{
			LevelComp->AddExp(50);
		}
	}
	
	DropItemToPlayer();
	
	MapManager::GetInstance()->SetMapObjectCoordinate(Position.Y, Position.X, {MapObjectType::Path, NO_ID});
	if (bIsBoss)
	{
		StageManager::GetInstance()->OnBossKilled(Position);
	}
	bIsDestroy = true;
}

void Monster::OnSpawnFromPool()
{
	AObject::OnSpawnFromPool();
	Health = MaxHealth;
	bIsBoss = false;
	AttackValue.clear();
	AttackVisibleTime = 0.0f;
	AttackTelegraphElapsedTime = 0.0f;
	bAttackTelegraphActive = false;
}

void Monster::OnReturnToPool()
{
	AObject::OnReturnToPool();
}

Player* Monster::FindPlayer()
{
	return SceneManager::GetInstance()->GetPlayer();
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

	Vector PlayerPos = player->GetPosition();
	Vector NextPosition = Position;
	
	int DiffX = PlayerPos.X - Position.X;
	int DiffY = PlayerPos.Y - Position.Y;

	
	if (abs(DiffX) > abs(DiffY))
	{
		if (DiffX > 0)
		{
			NextPosition.X++;
		}
		else if (DiffX < 0)
		{
			NextPosition.X--;
		}
	}
	
	else
	{
		if (DiffY > 0)
		{
			NextPosition.Y++;
		}
		else if (DiffY < 0)
		{
			NextPosition.Y--;
		}
	}

	if (NextPosition.X < 1 || NextPosition.X >= MAP_MAX_X - 1 || NextPosition.Y < 1 || NextPosition.Y >= MAP_MAX_Y - 1)
	{
		return;
	}

	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Wall)
		|| MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Shop)
		|| MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Monster)
		|| MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Player))
	{
		return;
	}

	BeginMoveTo(NextPosition);
}

void Monster::MoveRandom()
{
	int Random = rand() % 4;
	
	Vector Move[4]
	{
		{0, 1},
		{0, -1},
		{1, 0},
		{-1, 0},
	};
	
	Vector NextPosition;
	NextPosition.X = Position.X + Move[Random].X;
	NextPosition.Y = Position.Y + Move[Random].Y;
	
	if (NextPosition.X < 1 || NextPosition.X >= MAP_MAX_X - 1 || NextPosition.Y < 1 || NextPosition.Y >= MAP_MAX_Y - 1)
	{
		return;
	}
	
	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Wall))
	{
		return;
	}

	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Shop))
	{
		return;
	}
	
	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Monster))
	{
		return;
	}
	
	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Player))
	{
		return;
	}
	
	BeginMoveTo(NextPosition);
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
	
	const int MaxBFSRange = 15;
	
	const int LocalBFSSzie = MaxBFSRange * 2 + 1;
	
	int Range = DetectionRange;
	if (Range > MaxBFSRange)
	{
		Range = MaxBFSRange;
	}
	
	int VisitedRange = Range * 2 + 1;
	int VisitedMonster = VisitedRange / 2;
	
	int LocalMapX = Start.X - Range;
	int LocalMapY = Start.Y - Range;
	
	Vector LocalTarget;
	LocalTarget.X = Target.X - LocalMapX;
	LocalTarget.Y = Target.Y - LocalMapY;
	
	if (LocalTarget.X < 0 || LocalTarget.X >= VisitedRange || LocalTarget.Y < 0 || LocalTarget.Y >= VisitedRange)
	{
		MoveRandom();
		return;
	}
	
	queue<Vector> Q;
	bool visited[LocalBFSSzie][LocalBFSSzie] = {};
	Vector Parent[LocalBFSSzie][LocalBFSSzie];
	
	for (int Y = 0; Y < LocalBFSSzie; ++Y)
	{
		for (int X = 0; X < LocalBFSSzie; ++X)
		{
			Parent[Y][X] = Vector(-1, -1);
		}
	}
	
	Vector LocalStart;
	LocalStart.X = VisitedMonster;
	LocalStart.Y = VisitedMonster;
	
	Vector Move[4]
	{
		{0,1},
		{0,-1},
		{1,0},
		{-1,0},
	};
	
	visited[VisitedMonster][VisitedMonster] = true;
	Q.push(LocalStart);
	
	
	bool found = false;
	
	while (!Q.empty())
	{
		Vector Current = Q.front();
		Q.pop();
		
		if (Current.X == LocalTarget.X && Current.Y == LocalTarget.Y)
		{
			found = true;
			break;
		}
		for (int i = 0; i < 4; ++i)
		{
			Vector Go;
			
			Go.X = Current.X + Move[i].X;
			Go.Y = Current.Y + Move[i].Y;
			
			if (Go.X < 0 || Go.X >= VisitedRange || Go.Y < 0 || Go.Y >= VisitedRange)
			{
				continue;
			}
			
			if (visited[Go.Y][Go.X])
			{
				continue;
			}
			
			Vector WorldGo;
			WorldGo.X = Go.X + LocalMapX;
			WorldGo.Y = Go.Y + LocalMapY;
			
			if (WorldGo.X < 0 || WorldGo.X >= MAP_MAX_X || WorldGo.Y < 0 || WorldGo.Y >= MAP_MAX_Y)
			{
				continue;
			}
			
			if (MapManager::GetInstance()->IsTypeExist(WorldGo, MapObjectType::Wall))
			{
				continue;
			}
			if (MapManager::GetInstance()->IsTypeExist(WorldGo, MapObjectType::Monster))
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
	
	Vector Current = LocalTarget;
	
	while (!(Current.X == LocalStart.X && Current.Y == LocalStart.Y))
	{
		Moving.push_back(Current);
		
		Vector Before = Parent[Current.Y][Current.X];
		
		if (Before.X == -1 || Before.Y == -1)
		{
			return;
		}
		
		Current = Before;
	}
	
	Moving.push_back(LocalStart);
	
	reverse(Moving.begin(), Moving.end());
	
	if (Moving.size() < 2)
	{
		return;
	}
	
	Vector LocalNextPosition = Moving[1];
	
	Vector NextPosition;
	NextPosition.X = LocalNextPosition.X + LocalMapX;
	NextPosition.Y = LocalNextPosition.Y + LocalMapY;
	
	if (NextPosition.X == Target.X && NextPosition.Y == Target.Y)
	{
		return;
	}

	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Monster))
	{
		return;
	}
	
	if (MapManager::GetInstance()->IsTypeExist(NextPosition, MapObjectType::Player))
	{
		return;
	}
	
	BeginMoveTo(NextPosition);
}

bool Monster::CanAttackplayer(Player* player)
{
	if (player == nullptr)
	{
		return false;
	}
	
	if (player->IsDead())
	{
		return false;
	}
	
	int Distance = GetDistanceToPlayer(player);
	
	if (Distance > AttackRange)
	{
		return false;
	}
	
	if (AttackElapsedtime < AttackInterval)
	{
		return false;
	}

	if (bAttackTelegraphActive)
	{
		return false;
	}
	
	return true;
}

void Monster::Attackplayer(Player* player)
{
	if (player == nullptr)
	{
		return;
	}

	BuildAttackValue(player);
	if (AttackValue.empty())
	{
		return;
	}

	bAttackTelegraphActive = true;
	AttackTelegraphElapsedTime = 0.0f;
	AttackVisibleTime = AttackTelegraphDuration;
	AttackElapsedtime = 0.0f;
}

void Monster::BuildAttackValue(Player* player)
{
	if (player == nullptr)
	{
		return;
	}

	AttackValue.clear();
	Vector playerPosition = player->GetPosition();
	AttackValue.push_back(playerPosition);
	AttackValue.push_back({ playerPosition.X - 1, playerPosition.Y });
	AttackValue.push_back({ playerPosition.X + 1, playerPosition.Y });
	AttackValue.push_back({ playerPosition.X, playerPosition.Y - 1 });
	AttackValue.push_back({ playerPosition.X, playerPosition.Y + 1 });
}

void Monster::ExecutePendingAttack(Player* player)
{
	if (player == nullptr || player->IsDead())
	{
		return;
	}

	const Vector PlayerPosition = player->GetPosition();
	for (const Vector& AttackPosition : AttackValue)
	{
		if (AttackPosition.X == PlayerPosition.X && AttackPosition.Y == PlayerPosition.Y)
		{
			player->TakeDamage(Attack);
			return;
		}
	}
}

bool Monster::IsShiny() const
{
	return bIsShiny;
}

void Monster::ConfigureForStage(int InLevel, bool InBoss)
{
	if (InLevel <= 0)
	{
		InLevel = 1;
	}

	if (Name.find(L"강한 ") == 0)
	{
		Name = Name.substr(3);
	}

	bIsShiny = false;
	bIsBoss = InBoss;
	Level = InLevel;

	string typeName = GetName();
	if (typeName == "Goblin")
	{
		MaxHealth = Level * 45 + rand() % (Level * 15 + 1);
		Attack = Level * 6 + rand() % (Level * 3 + 1);
	}
	else if (typeName == "Slime")
	{
		MaxHealth = Level * 35 + rand() % (Level * 10 + 1);
		Attack = Level * 4 + rand() % (Level * 2 + 1);
	}
	else if (typeName == "Orc")
	{
		MaxHealth = Level * 70 + rand() % (Level * 20 + 1);
		Attack = Level * 10 + rand() % (Level * 5 + 1);
	}
	else if (typeName == "Spider")
	{
		MaxHealth = Level * 50 + rand() % (Level * 15 + 1);
		Attack = Level * 7 + rand() % (Level * 3 + 1);
	}
	else if (typeName == "KingSlime")
	{
		MaxHealth = Level * 220 + rand() % (Level * 60 + 1);
		Attack = Level * 25 + rand() % (Level * 5 + 1);
	}
	else if (typeName == "Mimic")
	{
		MaxHealth = Level * 180 + rand() % (Level * 50 + 1);
		Attack = Level * 30 + rand() % (Level * 6 + 1);
	}
	else if (typeName == "OrcMage")
	{
		MaxHealth = Level * 170 + rand() % (Level * 50 + 1);
		Attack = Level * 30 + rand() % (Level * 6 + 1);
	}
	else if (typeName == "Dragon")
	{
		MaxHealth = Level * 300 + rand() % (Level * 80 + 1);
		Attack = Level * 18 + rand() % (Level * 8 + 1);
	}
	else
	{
		MaxHealth = Level * 35 + rand() % (Level * 10 + 1);
		Attack = Level * 5 + rand() % (Level * 4 + 1);
	}

	if (bIsBoss)
	{
		MaxHealth = static_cast<int>(MaxHealth * 1.25f);
		//MaxHealth *= 2;
		Attack += Level * 5;
	}

	Health = MaxHealth;
	TrySetShiny();
}

void Monster::TrySetShiny()
{
	if (rand() % 30 == 0)
	{
		bIsShiny = true;
		
		//임시
		
		Name = L"강한 " + Name;
		
		MaxHealth += 100;
		Health = MaxHealth;
		Attack += 10;
	}
}

vector<FItemWeight> Monster::GetDropTable()
{
	return {};
}

void Monster::DropItemToPlayer()
{
	Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	
	if (PlayerPtr == nullptr)
	{
		return;
	}
	
	UInventoryComponent* InventoryComp = PlayerPtr->GetComponent<UInventoryComponent>();
	
	if (InventoryComp == nullptr)
	{
		return;
	}
	
	vector<FItemWeight> DropTable = GetDropTable();
	
	if (DropTable.empty())
	{
		return;
	}
	
	UItem* DroppedItem = ItemManager::GetInstance()->CreateRandomItem(DropTable);
	
	if (DroppedItem == nullptr)
	{
		return;
	}
	
	if (InventoryComp->AddItem(DroppedItem))
	{
		ViewportManager::GetInstance()->AddItemLog(DroppedItem);
	}
	else
	{
		delete DroppedItem;
	}
}



		


