// Misc.h

#pragma once


#include "Item.h"

struct Misc : public UItem
{
    Misc(string name, ItemType type, int price, int effectAmount = 0);
    ~Misc() {}
};