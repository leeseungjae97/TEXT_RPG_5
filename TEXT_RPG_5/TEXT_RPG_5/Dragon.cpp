#include "Dragon.h"
#include "Player.h"
#include "Manager/MapManager.h"
#include <cstdlib>

Dragon::Dragon(int PlayerLevel)
{
if (PlayerLevel <= 0)
{
    PlayerLevel = 1;
}

    Name = L"드래곤";
    Level = PlayerLevel;
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
    
    CurrentPhase = 1;
    //부활테스트
    bHasRevive = false;
    
    TrySetShiny();
}

//부활테스트
void Dragon::OnSpawnFromPool()
{
    Monster::OnSpawnFromPool();

    bHasRevive = false;
    CurrentPhase = 1;

    AttackInterval = 3.0f;
    MoveInterval = 1.2f;
    AttackElapsedtime = 0.0f;
    AttackVisibleTime = 0.0f;
    AttackValue.clear();

    Name = L"드래곤";
}

void Dragon::BuildAttackValue(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    UpdatePhase();
    
    AttackValue.clear();
    
    int Pattern = GetRandomPatternByPhase();
    
    if (Pattern == 0)
    {
        BreathAttack(player);
    }
    else if (Pattern == 1)
    {
        FireballAttack(player);
    }
    else if (Pattern == 2)
    {
        bool bRushAttackSuccess = RushAttack(player);
        
        if (!bRushAttackSuccess)
        {
            FireballAttack(player);
        }
    }
    else if (Pattern == 3)
    {
        PhaseThreeAttack(player);
    }
    
    
    // AttackVisibleTime = AttackVisibleDuration;
    
    // AttackElapsedtime = 0.0f;
}

void Dragon::BreathAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    Vector playerPosition = player->GetPosition();

    int diffX = playerPosition.X - Position.X;
    int diffY = playerPosition.Y - Position.Y;

    Vector direction = { 0, 0 };

    if (abs(diffX) > abs(diffY))
    {
        if (diffX > 0)
        {
            direction = { 1, 0 };
        }
        else
        {
            direction = { -1, 0 };
        }
    }
    else
    {
        if (diffY > 0)
        {
            direction = { 0, 1 };
        }
        else
        {
            direction = { 0, -1 };
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


void Dragon::FireballAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    Vector playerPosition = player->GetPosition();

    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            AttackValue.push_back({ playerPosition.X + x, playerPosition.Y + y });
        }
    }

    player->TakeDamage(Attack);
}


bool Dragon::RushAttack(Player* player)
{
    if (player == nullptr)
    {
        return false;
    }
    
    Vector PlayerPosition = player->GetPosition();
    
    bool bsameX = PlayerPosition.X == Position.X;
    bool bsameY = PlayerPosition.Y == Position.Y;
    
    if (!bsameX && !bsameY)
    {
        return false;
    }
    
    Vector Direction;
    
    if (bsameY)
    {
        if (PlayerPosition.X > Position.X)
        {
            Direction = {1, 0};
        }
        else
        {
            Direction = {-1, 0};
        }
    }
    else if (bsameX)
    {
        if (PlayerPosition.Y > Position.Y)
        {
            Direction = {0, 1};
        }
        else
        {
            Direction = {0, -1};
        }
    }
    
    Vector FinalPosition = Position;
    
    for (int i = 1; i <= AttackRange; ++i)
    {
        Vector CheckPosition;
        CheckPosition.X = Position.X + Direction.X * i;
        CheckPosition.Y = Position.Y + Direction.Y * i;
        
        AttackValue.push_back(CheckPosition);
        
        if (MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Wall) ||
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Shop) || 
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Monster) || 
            MapManager::GetInstance()->IsTypeExist(CheckPosition, MapObjectType::Crystal))
        {
            break;
        }
        
        if (CheckPosition.X == PlayerPosition.X && CheckPosition.Y == PlayerPosition.Y)
        {
            player->TakeDamage(Attack);
            break;
        }
        
        FinalPosition = CheckPosition;
    }
    
    if (!(FinalPosition.X == Position.X && FinalPosition.Y == Position.Y))
    {
        SetPosition(FinalPosition);
    }
    
    return true;
}

void Dragon::PhaseThreeAttack(Player* player)
{
    if (player == nullptr)
    {
        return;
    }

    Vector playerPosition = player->GetPosition();

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            AttackValue.push_back({ playerPosition.X + x, playerPosition.Y + y });
        }
    }

    player->TakeDamage(Attack + 10);
}


void Dragon::UpdatePhase()
{
    if (MaxHealth <= 0)
    {
        return;
    }
    
    //부활테스트
    if (bHasRevive)
    {
        CurrentPhase = 3;
        AttackInterval = 1.3f;
        MoveInterval = 0.7f;
        return;
    }
    
    float HealthRate = static_cast<float>(Health) / static_cast<float>(MaxHealth);
    
    if (HealthRate <= 0.3f)
    {
        CurrentPhase = 3;
        AttackInterval = 1.5f;
        MoveInterval = 0.8f;
    }
    else if (HealthRate <= 0.7f)
    {
        CurrentPhase = 2;
        AttackInterval = 2.2f;
        MoveInterval = 1.0f;
    }
    else
    {
        CurrentPhase = 1;
        AttackInterval = 3.0f;
        MoveInterval = 1.2f;
    }
}

int Dragon::GetRandomPatternByPhase()
{
    if (CurrentPhase == 1)
    {
        return rand() % 2;
    }
    if (CurrentPhase == 2)
    {
        return rand() % 3;
    }
    
    return rand() % 4;
}

//부활테스트
bool Dragon::Revive()
{
    if (bHasRevive)
    {
        return false;
    }
    
    bHasRevive = true;
    
    Health = MaxHealth / 2;
    
    CurrentPhase = 3;
    AttackInterval = 1.3f;
    MoveInterval = 0.7f;
    
    AttackElapsedtime = 0.0f;
    AttackValue.clear();
    AttackVisibleTime = 0.0f;
    
    Name = L"광폭한 드래곤";
    
    return true;
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


