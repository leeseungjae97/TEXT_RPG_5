#include "Slime.h"
#include <cstdlib>

Slime::Slime(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = "Slime";
	Health = playerLevel * 25 + rand() % (playerLevel * 10 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 3 + rand() % (playerLevel * 3 + 1);
	
	MoveInterval = 1.1f;

	DetectionRange = 3;

	//Position = Vector(3, 3);
	PrevPosition = Position;
	
	bUseBfs = true;
	
	AttackRange = 1;
	AttackInterval = 2.0f;
	AttackElapsedtime = 0.0f;
	
	TrySetShiny();
	
}

vector<FItemWeight> Slime::GetDropTable()
{
	return
	{	
		{ ItemId::SLIME_JELLY, 70 },
		{ ItemId::HP_POTION,  25 },
		{ ItemId::LONGSWORD, 25 },
		{ ItemId::BOW, 10 },
		{ ItemId::STAFF, 1},
	};
}
