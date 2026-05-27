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

public:
    UItem* CreateItem(ItemId Id);
    UItem* CreateRandomItem(const vector<FItemWeight>& Pool);
};
