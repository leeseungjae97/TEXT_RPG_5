#include "Slime.h"
#include <cstdlib>

Slime::Slime(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = "Slime";
	Health = playerLevel * 20 + rand() % (playerLevel * 10 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 5 + rand() % (playerLevel * 5 + 1);
	
	MoveInterval = 1.1f;

	
	DetectionRange = 3;

	Position = Vector(4, 4);
	PrevPosition = Position;
	
	bUseBfs = false;
}
