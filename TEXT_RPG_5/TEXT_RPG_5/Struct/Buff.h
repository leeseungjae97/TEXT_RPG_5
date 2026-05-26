#pragma once

#include "../Enum/StatType.h"

struct FBuff
{
    StatType Stat;
    int Amount;
    float Duration;
    float Elapsed = 0.0f;
};
