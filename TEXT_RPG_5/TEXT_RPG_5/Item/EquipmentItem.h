// EquipmentItem.h
#pragma once

#include "Item.h"

class EquipmentItem : public UItem
{
public:
    EquipmentItem(const FItemInfo& Info) : UItem(Info){}    
    ~EquipmentItem() {}


    void Use(Player* player) override;
};
