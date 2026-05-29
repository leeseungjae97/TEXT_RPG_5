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

UItem* ItemManager::CreateRandomItemWithRarity(const vector<FItemWeight>& Pool, ERarity Rarity)
{
    int Total = 0;
    for (const FItemWeight& Entry : Pool)
        Total += Entry.Weight;

    if (Total <= 0)
    {
        return nullptr;
    }

    int Roll = rand() % Total;
    int Acc = 0;
    for (const FItemWeight& Entry : Pool)
    {
        Acc += Entry.Weight;
        if (Roll < Acc)
            return CreateItemWithRarity(Entry.Id, Rarity);
    }

    return nullptr;
}

ERarity ItemManager::RollEquipmentRarity()
{
    int Roll = rand() % 100;   // 0~99
    if (Roll < 2)  return ERarity::Unique;   // 2%
    if (Roll < 10) return ERarity::Rare;     // 8%
    return ERarity::Common;                  // 90%
}

UItem* ItemManager::CreateItem(ItemId Id)
{
    UItem* Item = CreateBaseItem(Id);

    // 장비 아이템만 생성 시 1회 등급 부여
    if (Item != nullptr && Item->GetItemInfo().Type == ItemType::Equipment)
    {
        Item->SetRarity(RollEquipmentRarity());
    }

    return Item;
}

UItem* ItemManager::CreateItemWithRarity(ItemId Id, ERarity Rarity)
{
    UItem* Item = CreateBaseItem(Id);

    if (Item != nullptr)
    {
        Item->SetRarity(Rarity);
    }

    return Item;
}

UItem* ItemManager::CreateBaseItem(ItemId Id)
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

    case ItemId::FIRE_WALL_SCROLL:
        return new UsableItem(ItemDB::FIRE_WALL_SCROLL);

    case ItemId::LIGHTNING_STRIKE_SCROLL:
        return new UsableItem(ItemDB::LIGHTNING_STRIKE_SCROLL);

    case ItemId::FLAME_POTION:
        return new UsableItem(ItemDB::FLAME_POTION);
        
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

    case ItemId::EXPLORER_BOOTS:
        return new EquipmentItem(ItemDB::EXPLORER_BOOTS);

    case ItemId::PLATE_HELMET:
        return new EquipmentItem(ItemDB::PLATE_HELMET);

    case ItemId::PLATE_ARMOR:
        return new EquipmentItem(ItemDB::PLATE_ARMOR);

    case ItemId::BOOTS_OF_WIND:
        return new EquipmentItem(ItemDB::BOOTS_OF_WIND);

    default:
        return nullptr;
    }
}
