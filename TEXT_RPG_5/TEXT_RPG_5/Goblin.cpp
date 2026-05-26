#include "Goblin.h"
#include <cstdlib>

Goblin::Goblin(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = "Goblin";
	Health = playerLevel * 20 + rand() % (playerLevel * 10 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 5 + rand() % (playerLevel * 5 + 1);


	
	MoveInterval = 0.6f;

	
	DetectionRange = 7;

	Position = Vector(4, 4);
	PrevPosition = Position;
	
	bUseBfs = true;
	
}
