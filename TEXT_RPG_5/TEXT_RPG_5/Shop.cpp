#include "Shop.h"
#include "ItemDB.h"
#include "Item.h"

Shop::Shop()
{
	for (int i = 0; i < NumberOfItems; i++)
	{
        Container.push_back(GetRandomItem());
    }

    for (const UItem* Item : Container)
    {
        cout << Item->Name;
    }

}

static Shop::GetInstance()
{
    
}

const UItem* Shop::GetRandomItem()
{
    int n = rand() % 2;

    switch (n)
    {
    case 0:
        return &ItemDB::HP_POTION;

    case 1:
        return &ItemDB::STRENGTH_POTION;
    }

    return nullptr;
}
