#include "KingSlime.h"
#include "player.h"
#include <cstdlib>

KingSlime::KingSlime (int PlayerLevel)
{
    if (PlayerLevel <= 0)
    {
        PlayerLevel = 1;
    }
    
    Name = L"킹슬라임";
    Level = PlayerLevel;
    
    Health = PlayerLevel * 100 + rand() % (PlayerLevel * 40 + 1);
    MaxHealth = Health;

    Attack = PlayerLevel * 8 + rand() % (PlayerLevel * 5 + 1);

    MoveInterval = 1.5f;
    DetectionRange = 10;
    
    PrevPosition = Position;

    bUseBfs = true;

    AttackRange = 2;
    AttackInterval = 3.0f;
    AttackElapsedtime = 0.0f;

    TrySetShiny();
}

void KingSlime::Attackplayer(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    AttackValue.clear();
    
    int Pattern = rand() % 2;
    
    if (Pattern == 0)
    {
        GroundAttack(player);
    }
    else
    {
        CrossAttack(player);
    }
    
    AttackVisibleTime = AttackVisibleDuration;
    AttackElapsedtime = 0.0f;
}

void KingSlime::GroundAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    Vector PlayerPosition = player->GetPosition();
    
    bool bHitPlayer = false;
    
    for (int y = -2; y <= 2; y++)
    {
        for (int x = -2; x <= 2; x++)
        {
            Vector AttackPosition;
            AttackPosition.X = Position.X + x;
            AttackPosition.Y = Position.Y + y;
            
            AttackValue.push_back(AttackPosition);
            
            if (AttackPosition.X == PlayerPosition.X && AttackPosition.Y == PlayerPosition.Y)
            {
                bHitPlayer = true;
            }
        }
    }
    
    if (bHitPlayer)
    {
        player->TakeDamage(Attack);
    }
}

void KingSlime::CrossAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    Vector PlayerPosition = player->GetPosition();
    
    bool bHitPlayer = false;
    
    AttackValue.push_back(Position);
    
    if (Position.X == PlayerPosition.X && Position.Y == PlayerPosition.Y)
    {
        bHitPlayer = true;
    }
    
    for (int i = 1; i <= AttackRange; ++i)
    {
        Vector RightPosition = { Position.X + i, Position.Y };
        Vector LeftPosition = { Position.X - i, Position.Y };
        Vector DownPosition = { Position.X, Position.Y + i };
        Vector UpPosition = { Position.X, Position.Y - i };
        
        AttackValue.push_back(RightPosition);
        AttackValue.push_back(LeftPosition);
        AttackValue.push_back(DownPosition);
        AttackValue.push_back(UpPosition);
        
        if (RightPosition.X == PlayerPosition.X && RightPosition.Y == PlayerPosition.Y)
        {
            bHitPlayer = true;
        }
        if (LeftPosition.X == PlayerPosition.X && LeftPosition.Y == PlayerPosition.Y)
        {
            bHitPlayer = true;
        }
        if (DownPosition.X == PlayerPosition.X && DownPosition.Y == PlayerPosition.Y)
        {
            bHitPlayer = true;
        }
        if (UpPosition.X == PlayerPosition.X && UpPosition.Y == PlayerPosition.Y)
        {
            bHitPlayer = true;
        }
    }
    
    if (bHitPlayer)
    {
        player->TakeDamage(Attack);
    }
}


vector<FItemWeight> KingSlime::GetDropTable()
{
    return {
            { ItemId::SLIME_JELLY, 60 },
            { ItemId::HP_POTION,   30 },
            { ItemId::STAFF,       10 },
        };
}
