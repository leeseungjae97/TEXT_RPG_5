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
