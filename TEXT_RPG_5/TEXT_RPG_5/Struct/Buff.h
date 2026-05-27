#pragma once

#include "../Enum/StatType.h"
#include "../Enum/BuffType.h"

struct FBuff
{
    EBuffType BuffType  = EBuffType::Stat;

    // StatBuff
    StatType Stat       = StatType::None;
    int Amount          = 0;

    // PeriodicAttack
    int Damage          = 0;
    float Period        = 0.0f;
    float PeriodAcc     = 0.0f;

    // Common
    float Duration      = 0.0f;
    float Elapsed       = 0.0f;
};
