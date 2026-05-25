// Misc.h

#pragma once


#include "Struct/Item.h"
#include "Enum/ItemTypeEnum.h"

struct Misc : public UItem
{
    Misc(string name, ItemType type, int price, int effectAmount = 0);
    ~Misc() {}
};
