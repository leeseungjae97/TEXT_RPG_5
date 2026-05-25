//ItemManager.h
#pragma once

#include "../Singleton.h"
#include "../Item.h"
#include "../ItemIdEnum.h"

class ItemManager : public Singleton<ItemManager>
{
private:
    friend class Singleton<ItemManager>;

private:
    ItemManager();
    ~ItemManager();

public:
    UItem* CreateItem(ItemId Id);
};
