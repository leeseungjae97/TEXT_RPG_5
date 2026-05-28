#include "Mimic.h"
#include "Player.h"
#include "Define.h"
#include "Manager/MapManager.h"
#include <cstdlib>

Mimic::Mimic(int PlayerLevel)
{
    if (PlayerLevel <= 0)
    {
        PlayerLevel = 1;
    }

    Name = L"미믹";
    Level = PlayerLevel;

    Health = PlayerLevel * 80 + rand() % (PlayerLevel * 30 + 1);
    MaxHealth = Health;

    Attack = PlayerLevel * 12 + rand() % (PlayerLevel * 6 + 1);

    MoveInterval = 1.5f;
    DetectionRange = 10;

    PrevPosition = Position;

    
    bUseBfs = false;

    
    AttackRange = 6;

    AttackInterval = 3.0f;
    AttackElapsedtime = 0.0f;

    TrySetShiny();
    //RenderScale = 2;
}

void Mimic::BuildAttackValue(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    AttackValue.clear();

    RushWithCrossAttack();

    AttackVisibleTime = AttackVisibleDuration;
    AttackElapsedtime = 0.0f;
}

void Mimic::CrossRushAttack()
{
    Vector Directions[4]
    {
        { 0, -1 },  
        { 0,  1 },   
        { -1, 0 },   
        { 1,  0 }    
    };

    for (int d = 0; d < 4; ++d)
    {
        Vector Direction = Directions[d];

        for (int i = 1; i <= AttackRange; ++i)
        {
            Vector AttackPosition;
            AttackPosition.X = Position.X + Direction.X * i;
            AttackPosition.Y = Position.Y + Direction.Y * i;

            if (AttackPosition.X < 1 || AttackPosition.X >= MAP_MAX_X - 1 ||
                AttackPosition.Y < 1 || AttackPosition.Y >= MAP_MAX_Y - 1)
            {
                break;
            }

            if (MapManager::GetInstance()->IsTypeExist(AttackPosition, MapObjectType::Wall) ||
                MapManager::GetInstance()->IsTypeExist(AttackPosition, MapObjectType::Shop) ||
                MapManager::GetInstance()->IsTypeExist(AttackPosition, MapObjectType::Crystal))
            {
                break;
            }

            AddAttackCell(AttackPosition);
        }
    }
}

void Mimic::RushWithCrossAttack()
{
    
    CrossRushAttack();

   
    Vector Directions[4]
    {
        { 0, -1 },   
        { 0,  1 },   
        { -1, 0 },   
        { 1,  0 }    
    };

    int RandomIndex = rand() % 4;
    Vector Direction = Directions[RandomIndex];

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
    }
    Vector NextPosition;
    NextPosition.X = Position.X + Direction.X;
    NextPosition.Y = Position.Y + Direction.Y;

    if (!(NextPosition.X == Position.X && NextPosition.Y == Position.Y))
    {
        BeginMoveTo(NextPosition);
    }
    // 순간이동 느낌
    // if (!(FinalPosition.X == Position.X && FinalPosition.Y == Position.Y))
    // {
    //     BeginMoveTo(FinalPosition);
    // }
}

void Mimic::AddAttackCell(Vector Pos)
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

vector<FItemWeight> Mimic::GetDropTable()
{
    return
    {
        { ItemId::HP_POTION, 40 },
        { ItemId::LONGSWORD, 25 },
        { ItemId::STAFF, 20 },
        { ItemId::PLATE_ARMOR, 15 }
    };
}
