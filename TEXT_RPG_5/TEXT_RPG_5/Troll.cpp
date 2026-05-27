#include "Troll.h"
#include <cstdlib>

Troll::Troll(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = L"Troll";
	Level = playerLevel;
	Health = playerLevel * 50 + rand() % (playerLevel * 20 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 9 + rand() % (playerLevel * 6 + 1);
	
	MoveInterval = 1.3f;

	
	DetectionRange = 12;

	//Position = Vector(4, 4);
	PrevPosition = Position;
	
	bUseBfs = true;
	
	AttackRange = 1;
	AttackInterval = 2.4f;
	AttackElapsedtime = 0.0f;
	
}
