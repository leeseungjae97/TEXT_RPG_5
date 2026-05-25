// HP_Potion.h
#pragma once

#include "Struct/Item.h"

struct HP_Potion : public UItem
{
    HP_Potion(string name, ItemType type, int price, int effectAmount = 0);
    ~HP_Potion() {}


    void Use(Player* player) override;
};
