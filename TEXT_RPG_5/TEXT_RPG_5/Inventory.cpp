//Inventory.cpp

#include "Inventory.h"


void Inventory::printInventory()
{
    int currentCount = 0;
    std::cout << "\n[ 인벤토리 (" << Container.size() << "/" << "10" << ") ]\n";
    currentCount = 0;
    for (Item* item : Container)
    {
        std::cout << ++currentCount << ". ";
        item->printInfo();
    }
}


void Inventory::AddItem(Item* item)
{
    Container.push_back(item);
    cout << "-> " << item->Name << "을(를) 획득하였습니다.\n";
}

bool Inventory::RemoveItem(Item* item)
{ 
    auto it = find(Container.begin(), Container.end(), item);
    if (it != Container.end())
    {
        Container.erase(it);
        return true;
    }

    return false;
}

bool Inventory::UseItem(Item* item)
{
    if (item->Type == ItemType::Usable)
    {
        item->Use(Player);
        return true;
    }
    else
    {
        cout << "\n 사용할 수 없는 아이템입니다.\n";
        return false;
    }
}

