#include "ItemManager.h"

#include "../ItemDB.h"

#include "../UsableItem.h"
#include "../EquipmentItem.h"
#include "../MiscItem.h"

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

    default:
        return nullptr;
    }
}
