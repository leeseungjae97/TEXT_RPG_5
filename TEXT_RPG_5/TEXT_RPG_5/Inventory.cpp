//Inventory.cpp

#include "Inventory.h"

InventoryManager* InventoryManager::Instance = nullptr;

void InventoryManager::OpenInventory()
{
    int currentCount = 0;
    for (UItem* Item : Container)
    {
        ++currentCount;
        Item->printInfo();
    }
}


vector<UItem*>& InventoryManager::GetContainer()
{
    return Container;

}

int InventoryManager::GetItemIndex(UItem* Item)
{
    auto it = find(Container.begin(), Container.end(), Item);
    if (it != Container.end())
        return it - Container.begin();
    else
        return -1;
}

void InventoryManager::AddItem(UItem* Item)
{
    Container.push_back(Item);
}

bool InventoryManager::RemoveItem(UItem* Item)
{ 
    auto it = find(Container.begin(), Container.end(), Item);
    if (it != Container.end())
    {
        Container.erase(it);
        return true;
    }

    return false;
}

bool InventoryManager::UseItem()
{
    int n = rand() % Container.size();
    UItem* Item = Container[n];

    if (Item->Type == ItemType::Usable)
    {
        Item->Use(Player);
        RemoveItem(Item);
        return true;
    }
    else
    {
        return false;
    }
}

