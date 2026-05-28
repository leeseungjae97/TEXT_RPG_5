
#include "Spider.h"
#include "player.h"
#include <cstdlib>

Spider::Spider(int playerLevel)
{
    if (playerLevel <= 0)
    {
        playerLevel = 1;
    }

    Name = L"거미";
    Level = playerLevel;
    Health = playerLevel * 30 + rand() % (playerLevel * 10 + 1);
    MaxHealth = Health;
    Attack = playerLevel * 4 + rand() % (playerLevel * 4 + 1);
	
    MoveInterval = 0.7f;

    DetectionRange = 8;

    //Position = Vector(3, 3);
    PrevPosition = Position;
	
    bUseBfs = true;
	
    AttackRange = 2;
    AttackInterval = 1.8f;
    AttackElapsedtime = 0.0f;
    
    TrySetShiny();
}

void Spider::BuildAttackValue(Player* player)
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
        AttackValue.push_back(playerPosition);
    
        for (int i = 1; i <=2; ++i)
        {
            AttackValue.push_back({ playerPosition.X + i, playerPosition.Y });
            AttackValue.push_back({ playerPosition.X - i, playerPosition.Y });
            AttackValue.push_back({ playerPosition.X, playerPosition.Y + i });
            AttackValue.push_back({ playerPosition.X, playerPosition.Y - i });
        }
    }
    else
    {
        int DiffX = playerPosition.X - Position.X;
        int DiffY = playerPosition.Y - Position.Y;
        
        Vector Direction;
        
        if (abs(DiffX)>abs(DiffY))
        {
            if (DiffX > 0)
            {
                Direction = {1, 0};
            }
            else
            {
                Direction = {-1, 0};
            }
        }
        else
        {
            if (DiffY > 0)
            {
                Direction = {0, 1};
            }
            else
            {
                Direction = {0, -1};
            }
        }
        for (int i = 1; i <= AttackRange; ++i)
        {
            Vector attackPosition;
            attackPosition.X = Position.X + Direction.X * i;
            attackPosition.Y = Position.Y + Direction.Y * i;

            AttackValue.push_back(attackPosition);
        }
    }
}

vector<FItemWeight> Spider::GetDropTable()
{
    return 
    {
        { ItemId::SPIDER_EYE, 70 },
        { ItemId::HP_POTION,  20 },
        { ItemId::BOW,        10 },
        { ItemId::STAFF, 5},
        
        };
}
