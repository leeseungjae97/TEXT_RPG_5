#include "OrcMage.h"
#include "Player.h"
#include "Define.h"
#include "pch.h"
#include <cstdlib>

OrcMage::OrcMage(int PlayerLevel)
{
    if (PlayerLevel <= 0)
    {
        PlayerLevel = 1;
    }

    Name = L"오크메이지";
    Level = PlayerLevel;

    Health = PlayerLevel * 70 + rand() % (PlayerLevel * 25 + 1);
    MaxHealth = Health;

    Attack = PlayerLevel * 10 + rand() % (PlayerLevel * 6 + 1);

    MoveInterval = 1.2f;
    DetectionRange = 12;

    PrevPosition = Position;

    bUseBfs = true;
    
    AttackRange = 7;
    AttackInterval = 3.0f;
    AttackElapsedtime = 0.0f;

    //폭발사이즈테스트
    ExplosionRadius = 1;
    PatternScale = 2;

    TrySetShiny();
    RenderScale = 2;
}

void OrcMage::BuildAttackValue(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    AttackValue.clear();
    
    int pattern = rand() % 2;
    
    if (pattern == 0)
    {
        BuildSpiralPattern(player->GetPosition());
    }
    else
    {
        BuildStarPattern(player->GetPosition());
    }
    
    AttackVisibleTime = AttackTelegraphDuration;
    AttackElapsedtime = 0.0f;
    
}

void OrcMage::AddAttackCell(Vector Pos)
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

void OrcMage::AddPattern(Vector Center, const vector<Vector>& Offsets)
{
    for (const Vector& Offset : Offsets)
    {
        Vector Pos;
        Pos.X = Center.X + Offset.X * PatternScale;
        Pos.Y = Center.Y + Offset.Y * PatternScale;

        if (ExplosionRadius <= 0)
        {
            AddAttackCell(Pos);
        }
        else
        {
            for (int y = -ExplosionRadius; y <= ExplosionRadius; ++y)
            {
                for (int x = -ExplosionRadius; x <= ExplosionRadius; ++x)
                {
                    Vector ExpandPos;
                    ExpandPos.X = Pos.X + x;
                    ExpandPos.Y = Pos.Y + y;
                    AddAttackCell(ExpandPos);
                }
            }
        }
    }
}

void OrcMage::BuildStarPattern(Vector Center)
{
    vector<Vector> Offsets =
    {
        { 0, -3 },

        { -2, -1 },
        {  2, -1 },

        { -3,  0 },
        {  0,  0 },
        {  3,  0 },

        { -2,  1 },
        {  2,  1 },

        { 0,  3 }
    };

    AddPattern(Center, Offsets);
}

void OrcMage::BuildSpiralPattern(Vector Center)
{
    vector<Vector> Offsets =
    {
        { -3, -3 }, { -1, -3 }, {  1, -3 }, {  3, -3 },

                                                       {  3, -2 },

       { -3, -1 },                         {  1, -1 }, {  3, -1 },

       { -3,  0 },             {  0,  0 },

       { -3,  1 }, { -1,  1 }, {  1,  1 }, {  3,  1 },

       { -3,  2 },

       { -3,  3 }, { -1,  3 }, {  1,  3 }, {  3,  3 }
    };

    AddPattern(Center, Offsets);
}

vector<FItemWeight> OrcMage::GetDropTable()
{
    return
    {
            { ItemId::ORC_TUSK, 50 },
            { ItemId::STAFF, 25 },
            { ItemId::FLAME_POTION, 30 },
            { ItemId::LIGHTNING_STRIKE_SCROLL, 30 },
            { ItemId::FIRE_WALL_SCROLL, 30}
    };
}
