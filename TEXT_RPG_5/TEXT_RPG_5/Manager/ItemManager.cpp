#include "ItemManager.h"

#include "../Item/ItemDB.h"

#include "../Item/UsableItem.h"
#include "../Item/EquipmentItem.h"
#include "../Item/MiscItem.h"

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
}

UItem* ItemManager::CreateItem(ItemId Id)
{
    switch (Id)
    {
    case ItemId::HP_POTION:
        return new UsableItem(ItemDB::HP_POTION);

    case ItemId::STRENGTH_POTION:
        return new UsableItem(ItemDB::STRENGTH_POTION);
        
    case ItemId::GOBLIN_LEATHER:
        return new MiscItem(ItemDB::GOBLIN_LEATHER);

    case ItemId::LONGSWORD:
        return new EquipmentItem(ItemDB::LONGSWORD);

    default:
        return nullptr;
    }
}
