#include "Orc.h"
#include <cstdlib>

Orc::Orc(int playerLevel)
{
	if (playerLevel <= 0)
	{
		playerLevel = 1;
	}

	Name = "Orc";
	Health = playerLevel * 20 + rand() % (playerLevel * 10 + 1);
	MaxHealth = Health;
	Attack = playerLevel * 5 + rand() % (playerLevel * 5 + 1);
}
