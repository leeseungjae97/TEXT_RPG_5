//ItemManager.h
#pragma once

#include "../Singleton.h"
#include "../Item/Item.h"
#include "../Enum/ItemIdEnum.h"
#include "../Struct/ItemWeight.h"

class ItemManager : public Singleton<ItemManager>
{
private:
    friend class Singleton<ItemManager>;

private:
    ItemManager();
    ~ItemManager();

    UItem* CreateBaseItem(ItemId Id);
    ERarity RollEquipmentRarity();

public:
    UItem* CreateItem(ItemId Id);
    UItem* CreateItemWithRarity(ItemId Id, ERarity Rarity);
    UItem* CreateRandomItem(const vector<FItemWeight>& Pool);
    UItem* CreateRandomItemWithRarity(const vector<FItemWeight>& Pool, ERarity Rarity);
};
