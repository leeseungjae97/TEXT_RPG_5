//Inventory.cpp

#include "Inventory.h"
#include "Item.h"


void InventoryManager::SetOwner(Player* Owner) { this->Owner = Owner; }

Player* InventoryManager::GetOwner() { return Owner; }

void InventoryManager::OpenInventory()
{
    int currentCount = 0;
    for (UItem* Item : Container)
    {
        ++currentCount;
        Item->printInfo();
    }
}


int InventoryManager::GetGold()
{
    return Gold;
}

void InventoryManager::AddGold(int Amount)
{
    Gold += Amount;
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

UItem* InventoryManager::GetItem(int Index)
{
    return Container[Index];
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

bool InventoryManager::UseRandomItem()
{
    int n = rand() % Container.size();
    UItem* Item = Container[n];

    if (Item->Type == ItemType::Usable)
    {
        Item->Use(GetOwner());
        RemoveItem(Item);
        return true;
    }
    else
    {
        return false;
    }
}

bool InventoryManager::UseItem(UItem* Item)
{

    if (Item->Type == ItemType::Usable)
    {
        Item->Use(GetOwner());
        RemoveItem(Item);
        return true;
    }
    else
    {
        return false;
    }
}




map<int, UItem*> InventoryManager::GetQuickSlot()
{
    return QuickSlot;
}


void InventoryManager::RegisterOnQuickSlot(int Number, UItem* Item)
{
    QuickSlot[Number] = Item;
}

UItem* InventoryManager::GetItemFromQuickSlot(int Number)
{
    return QuickSlot[Number];
}

void InventoryManager::UseQuickSlot(int Number)
{
    UseItem(QuickSlot[Number]);
}




void InventoryManager::BuyItem(UItem* Item)
{
    if (Gold >= Item->Price)
    {
        AddGold(-(Item->Price));
        AddItem(Item);
    }
    else
    {
        
    }
}

void InventoryManager::SellItem(UItem* Item)
{
    if (RemoveItem(Item))
        AddGold(Item->Price);
}
