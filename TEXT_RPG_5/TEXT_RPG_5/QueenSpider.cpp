#include "QueenSpider.h"
#include "Player.h"
#include "Manager/MapManager.h"
#include "Define.h"
#include <cstdlib>

QueenSpider::QueenSpider(int PlayerLevel)
{
    if (PlayerLevel <= 0)
    {
        PlayerLevel = 1;
    }

    Name = L"퀸스파이더";
    Level = PlayerLevel;

    Health = PlayerLevel * 260 + rand() % (PlayerLevel * 70 + 1);
    MaxHealth = Health;

    Attack = PlayerLevel * 24 + rand() % (PlayerLevel * 8 + 1);

    MoveInterval = 0.9f;
    DetectionRange = 12;

    PrevPosition = Position;

    bUseBfs = true;

    AttackRange = 11;
    AttackInterval = 2.8f;
    AttackElapsedtime = 0.0f;

    TrySetShiny();

    RenderScale = 3;
}

void QueenSpider::BuildAttackValue(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    AttackValue.clear();
    AttackTileType = EMonsterAttackTileType::Default;

    int Pattern = rand() % 2;

    if (Pattern == 0)
    {
        bool bRushSuccess = RushAttack(player);

        if (!bRushSuccess)
        {
            WebAreaAttack(player);
        }
    }
    else
    {
       WebAreaAttack(player);
    }

    AttackVisibleTime = AttackVisibleDuration;
    AttackElapsedtime = 0.0f;
}

bool QueenSpider::RushAttack(Player* player)
{
    if (player == nullptr)
    {
        return false;
    }

    Vector PlayerPosition = player->GetPosition();

    bool bSameX = PlayerPosition.X == Position.X;
    bool bSameY = PlayerPosition.Y == Position.Y;

    if (!bSameX && !bSameY)
    {
        return false;
    }

    Vector Direction;

    if (bSameY)
    {
        if (PlayerPosition.X > Position.X)
        {
            Direction = { 1, 0 };
        }
        else
        {
            Direction = { -1, 0 };
        }
    }
    else
    {
        if (PlayerPosition.Y > Position.Y)
        {
            Direction = { 0, 1 };
        }
        else
        {
            Direction = { 0, -1 };
        }
    }

    Vector FinalPosition = Position;

    for (int i = 1; i <= AttackRange; ++i)
    {
        Vector CheckPosition;
        CheckPosition.X = Position.X + Direction.X * i;
        CheckPosition.Y = Position.Y + Direction.Y * i;

        
        if (CheckPosition.X < 1 || CheckPosition.X >= MAP_MAX_X - 1 ||
            CheckPosition.Y < 1 || CheckPosition.Y >= MAP_MAX_Y - 1)
        {
            break;
        }

        
        if (MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Wall) ||
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Shop) ||
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Monster) ||
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Crystal))
        {
            break;
        }

        
        AddAttackCell(CheckPosition);

       
        FinalPosition = CheckPosition;

        
        if (CheckPosition.X == PlayerPosition.X && CheckPosition.Y == PlayerPosition.Y)
        {
            break;
        }
    }

    if (!(FinalPosition.X == Position.X && FinalPosition.Y == Position.Y))
    {
        SetPosition(FinalPosition);
    }

    return true;
}

//쉬운버전 거미줄
void QueenSpider::WebAreaAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    Vector Center = player->GetPosition();

    
    AddAttackCell(Center);

   
    for (int i = 1; i <= 4; ++i)
    {
        AddAttackCell({ Center.X + i, Center.Y });
        AddAttackCell({ Center.X - i, Center.Y });
    }

    
    for (int i = 1; i <= 4; ++i)
    {
        AddAttackCell({ Center.X, Center.Y + i });
        AddAttackCell({ Center.X, Center.Y - i });
    }

    
    AddAttackCell({ Center.X - 3, Center.Y - 3 });
    AddAttackCell({ Center.X + 3, Center.Y - 3 });
    AddAttackCell({ Center.X - 3, Center.Y + 3 });
    AddAttackCell({ Center.X + 3, Center.Y + 3 });
}

// void QueenSpider::WebAreaAttack(Player* player)
// {
//     if (player == nullptr)
//     {
//         return;
//     }
//
//     
//     Vector Center = player->GetPosition();
//
//     AddAttackCell(Center);
//
//     for (int i = 1; i <= 4; ++i)
//     {
//         AddAttackCell({ Center.X + i, Center.Y });
//         AddAttackCell({ Center.X - i, Center.Y });
//     }
    
//     for (int i = 1; i <= 4; ++i)
//     {
//         AddAttackCell({ Center.X, Center.Y + i });
//         AddAttackCell({ Center.X, Center.Y - i });
//     }
//
//     
//     AddAttackCell({ Center.X - 1, Center.Y - 1 });
//     AddAttackCell({ Center.X + 1, Center.Y - 1 });
//     AddAttackCell({ Center.X - 1, Center.Y + 1 });
//     AddAttackCell({ Center.X + 1, Center.Y + 1 });
//
//     
//     AddAttackCell({ Center.X - 3, Center.Y - 3 });
//     AddAttackCell({ Center.X + 3, Center.Y - 3 });
//     AddAttackCell({ Center.X - 3, Center.Y + 3 });
//     AddAttackCell({ Center.X + 3, Center.Y + 3 });
//
//     AddAttackCell({ Center.X - 2, Center.Y - 1 });
//     AddAttackCell({ Center.X + 2, Center.Y - 1 });
//     AddAttackCell({ Center.X - 2, Center.Y + 1 });
//     AddAttackCell({ Center.X + 2, Center.Y + 1 });
//
//     AddAttackCell({ Center.X - 1, Center.Y - 2 });
//     AddAttackCell({ Center.X + 1, Center.Y - 2 });
//     AddAttackCell({ Center.X - 1, Center.Y + 2 });
//     AddAttackCell({ Center.X + 1, Center.Y + 2 });
// }

void QueenSpider::AddAttackCell(Vector Pos)
{
    if (Pos.X < 0 || Pos.X >= MAP_MAX_X ||
        Pos.Y < 0 || Pos.Y >= MAP_MAX_Y)
    {
        return;
    }

    for (const Vector& Value : AttackValue)
    {
        if (Value.X == Pos.X && Value.Y == Pos.Y)
        {
            return;
        }
    }

    AttackValue.push_back(Pos);
}

vector<FItemWeight> QueenSpider::GetDropTable()
{
    return
    {
        { ItemId::SPIDER_EYE, 50 },
        { ItemId::HP_POTION, 25 },
        { ItemId::BOW, 15 },
        { ItemId::STRENGTH_POTION, 10 }
    };
}
