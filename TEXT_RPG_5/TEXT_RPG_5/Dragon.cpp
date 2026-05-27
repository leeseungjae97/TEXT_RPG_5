#include "Dragon.h"
#include "Player.h"
#include <cstdlib>

Dragon::Dragon(int PlayerLevel)
{
if (PlayerLevel <= 0)
{
    PlayerLevel = 1;
}

    Name = "Dragon";
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
    
    TrySetShiny();
}

void Dragon::Attackplayer(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    AttackValue.clear();
    
    Vector playerPosition = player->GetPosition();
    
    int Pattern = rand() % 2;
    if (Pattern == 0)
    {
        int diffX = playerPosition.X - Position.X;
        int diffY = playerPosition.Y - Position.Y;
    
        Vector direction;
    
        if (abs(diffX) > abs(diffY))
        {
            if (diffX > 0)
            {
                direction = {1, 0};
            }
            else
            {
                direction = {-1, 0};
            }
        }
        else
        {
            if (diffY > 0)
            {
                direction = {0, 1};
            }
            else
            {
                direction = {0, -1};
            }
        }
    
        for (int i = 1; i <= AttackRange; ++i)
        {
            Vector AttackPosition;
            AttackPosition.X = Position.X + direction.X * i;
            AttackPosition.Y = Position.Y + direction.Y * i;
        
            AttackValue.push_back(AttackPosition);
        }
        player->TakeDamage(Attack);
        
    }
    else
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int x = -1; x <= 1; ++x)
            {
                AttackValue.push_back({ playerPosition.X + x, playerPosition.Y + y });
            }
        }
        
        player->TakeDamage(Attack);
    }
    
   
    
    AttackVisibleTime = AttackVisibleDuration;
    
    AttackElapsedtime = 0.0f;
}

vector<FItemWeight> Dragon::GetDropTable()
{
    return {
		    { ItemId::DRAGON_HEART,     55 },
            { ItemId::STRENGTH_POTION,  25 },
            { ItemId::PLATE_ARMOR,      20 },
            { ItemId::STAFF,            10 },
        };
}
