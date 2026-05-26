// EquipmentItem.cpp

#include "EquipmentItem.h"
#include "../Player.h"
#include "ItemDB.h"




void EquipmentItem::Use(Player* player)
{
    UItem::Use(player);
    
    switch (ItemInfo.Id)
    {
    case ItemId::LONGSWORD:
        break;
        
        
    }
}
