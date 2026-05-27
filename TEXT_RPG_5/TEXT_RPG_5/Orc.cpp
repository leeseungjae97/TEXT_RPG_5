#include "Orc.h"
#include <cstdlib>

Orc::Orc(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = "Orc";
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
	
}
