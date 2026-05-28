//RarityEnum.h

#pragma once

enum class ERarity
{
    Common,   // 일반
    Rare,     // 희귀
    Unique,   // 유니크

    MAX
};

inline const wchar_t* GetRarityName(ERarity Rarity)
{
    switch (Rarity)
    {
    case ERarity::Rare:   return L"희귀";
    case ERarity::Unique: return L"유니크";
    case ERarity::Common:
    default:              return L"일반";
    }
}

// 등급별 스탯/가격 배수 (조정은 여기서)
inline float GetRarityStatMultiplier(ERarity Rarity)
{
    switch (Rarity)
    {
    case ERarity::Rare:   return 1.5f;
    case ERarity::Unique: return 2.0f;
    case ERarity::Common:
    default:              return 1.0f;
    }
}

inline float GetRarityPriceMultiplier(ERarity Rarity)
{
    switch (Rarity)
    {
    case ERarity::Rare:   return 2.0f;
    case ERarity::Unique: return 4.0f;
    case ERarity::Common:
    default:              return 1.0f;
    }
}
