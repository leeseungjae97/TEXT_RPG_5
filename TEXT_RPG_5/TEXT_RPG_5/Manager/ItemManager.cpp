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


UItem* ItemManager::CreateRandomItem(const vector<FItemWeight>& Pool)
{
    int Total = 0;
    for (const FItemWeight& Entry : Pool)
        Total += Entry.Weight;

    int Roll = rand() % Total;
    int Acc = 0;
    for (const FItemWeight& Entry : Pool)
    {
        Acc += Entry.Weight;
        if (Roll < Acc)
            return CreateItem(Entry.Id);
    }

    return nullptr;
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
    
    case ItemId::SLIME_JELLY:
        return new MiscItem(ItemDB::SLIME_JELLY);
        
    case ItemId::ORC_TUSK:
        return new MiscItem(ItemDB::ORC_TUSK);
        
    case ItemId::SPIDER_EYE:
        return new MiscItem(ItemDB::SPIDER_EYE);
        
    case ItemId::DRAGON_HEART:
        return new UsableItem(ItemDB::DRAGON_HEART);
        
    case ItemId::LONGSWORD:
        return new EquipmentItem(ItemDB::LONGSWORD);

    case ItemId::BOW:
        return new EquipmentItem(ItemDB::BOW);

    case ItemId::AXE:
        return new EquipmentItem(ItemDB::AXE);

    case ItemId::STAFF:
        return new EquipmentItem(ItemDB::STAFF);

    case ItemId::LEATHER_HELMET:
        return new EquipmentItem(ItemDB::LEATHER_HELMET);

    case ItemId::LEATHER_ARMOR:
        return new EquipmentItem(ItemDB::LEATHER_ARMOR);

    case ItemId::LEATHER_BOOTS:
        return new EquipmentItem(ItemDB::LEATHER_BOOTS);

    case ItemId::PLATE_HELMET:
        return new EquipmentItem(ItemDB::PLATE_HELMET);

    case ItemId::PLATE_ARMOR:
        return new EquipmentItem(ItemDB::PLATE_ARMOR);

    case ItemId::PLATE_BOOTS:
        return new EquipmentItem(ItemDB::PLATE_BOOTS);

    default:
        return nullptr;
    }
}
