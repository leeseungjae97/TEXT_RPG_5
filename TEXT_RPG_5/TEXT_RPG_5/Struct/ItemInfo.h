//ItemInfo.h

#pragma once

#include "../pch.h"
#include "../Enum/ItemTypeEnum.h"
#include "../Enum/ItemIdEnum.h"
#include "../Enum/StatType.h"
#include "../Enum/EquipmentTypeEnum.h"
#include "../Enum/WeaponType.h"
#include "../Enum/RarityEnum.h"


struct FItemInfo
{
    string Name;
    ItemType Type;
    ItemId Id;
    int Price;
    int EffectAmount;
    StatType EffectType = StatType::None;
    EquipmentType EquipSlot = EquipmentType::NONE;
    WeaponType AttackType = WeaponType::NONE;
    ERarity Rarity = ERarity::Common;
};
