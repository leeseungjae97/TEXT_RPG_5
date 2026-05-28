#include "Goblin.h"
#include <cstdlib>

Goblin::Goblin(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = L"고블린";
	Level = playerLevel;
	Health = playerLevel * 20 + rand() % (playerLevel * 10 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 4 + rand() % (playerLevel * 4 + 1);


	
	MoveInterval = 0.6f;

	
	DetectionRange = 7;

	//Position = Vector(4, 5);
	PrevPosition = Position;
	
	bUseBfs = true;
	
	AttackRange = 1;
	AttackInterval = 1.1f;
	AttackElapsedtime = 0.0f;
	
	TrySetShiny();
}

std::vector<FItemWeight> Goblin::GetDropTable()
{
	return
	{
		{ ItemId::GOBLIN_LEATHER, 80},
		{ ItemId::HP_POTION, 20},
		{ItemId::STRENGTH_POTION, 10},
		{ ItemId::LEATHER_ARMOR, 5},
		{ ItemId::LEATHER_HELMET, 5},
		{ ItemId::LEATHER_BOOTS, 5},
		
	};
}
