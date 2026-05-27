#include "Orc.h"
#include "Player.h"
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
	
	//TrySetShiny();
}
	
void Orc::Attackplayer(Player* player)
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
	
	AttackVisibleTime = AttackVisibleDuration;
	
	player->TakeDamage(Attack);
	
	AttackElapsedtime = 0.0f;
	
}
	
		
	
	

