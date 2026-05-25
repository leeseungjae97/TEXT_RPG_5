//ItemInfo.h

#pragma once

#include "../pch.h"
#include "../Enum/ItemTypeEnum.h"
#include "../Enum/ItemIdEnum.h"


struct FItemInfo
{
    string Name;
    ItemType Type;
    ItemId Id;
    int Price;
    int EffectAmount;
	
};
