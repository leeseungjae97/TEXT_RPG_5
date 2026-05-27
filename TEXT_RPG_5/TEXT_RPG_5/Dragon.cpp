#include "Dragon.h"
#include <cstdlib>

Dragon::Dragon(int PlayerLevel)
{
if (PlayerLevel <= 0)
{
    PlayerLevel = 1;
}

    Name = "Slime";
    Health = PlayerLevel * 80 + rand() % (PlayerLevel * 30 + 1);
    MaxHealth = Health;
    Attack = PlayerLevel * 12 + rand() % (PlayerLevel * 8 + 1);
	
    MoveInterval = 1.2f;

    DetectionRange = 12;

    Position = Vector(3, 3);
    PrevPosition = Position;
	
    bUseBfs = true;
	
    AttackRange = 4;
    AttackInterval = 3.0f;
    AttackElapsedtime = 0.0f;
}
