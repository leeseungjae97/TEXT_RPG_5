#include "Orc.h"
#include "Player.h"
#include <cstdlib>

Orc::Orc(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = L"오크";
	Level = playerLevel;
	Health = playerLevel * 35 + rand() % (playerLevel * 15 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 6 + rand() % (playerLevel * 5 + 1);
	
	MoveInterval = 0.9f;
	
	
	DetectionRange = 9;

	//Position = Vector(3, 5);
	PrevPosition = Position;
	
	bUseBfs = true;
	
	AttackRange = 1;
	AttackInterval = 1.6f;
	AttackElapsedtime = 0.0f;
	
	TrySetShiny();
}

void Orc::BuildAttackValue(Player* player)
{
	if (player == nullptr)
	{
		return;
	}
	
	AttackValue.clear();
	
	Vector playerPosition = player->GetPosition();
	
	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			AttackValue.push_back({ playerPosition.X + x, playerPosition.Y + y });
		}
	}
}

vector<FItemWeight> Orc::GetDropTable()
{
	return
	{
		{ ItemId::ORC_TUSK, 60},
		{ ItemId::STRENGTH_POTION, 20},
		{ ItemId::PLATE_HELMET, 3},
		{ ItemId::PLATE_ARMOR, 3},
		{ ItemId::PLATE_BOOTS, 3},
		{ ItemId::AXE, 30}
		
		};
}
