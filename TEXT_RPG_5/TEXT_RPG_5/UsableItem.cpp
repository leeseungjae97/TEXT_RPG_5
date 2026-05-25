// UsableItem.cpp

#include "UsableItem.h"
#include "Player.h"
#include "ItemDB.h"




void UsableItem::Use(Player* player)
{
    UItem::Use(player);
    
    switch (ItemInfo.Id)
    {
    case ItemId::HP_POTION:
        player->SetHP(min(player->GetMax_HP(), (player->GetHP() + ItemInfo.EffectAmount)));
        break;
        
    case ItemId::STRENGTH_POTION:
        break;
        
        
        
    }
    
}
