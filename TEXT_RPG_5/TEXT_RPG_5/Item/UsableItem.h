// UsableItem.h
#pragma once

#include "Item.h"

class UsableItem : public UItem
{
public:
    UsableItem(const FItemInfo& Info) : UItem(Info){}
    ~UsableItem() {}

    void Use(Player* player) override;
};
