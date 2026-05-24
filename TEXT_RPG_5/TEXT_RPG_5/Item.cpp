// Item.cpp
#include "Item.h"
#include "Player.h"

UItem::UItem(string name, ItemType type, int price, int effectAmount)
{
    Name = name;
    Type = type;
    Price = price;
    EffectAmount = effectAmount;
}

void UItem::printInfo() const
{
}

void UItem::Use(Player* player)
{
}

void UItem::Tick(float DeltaTime)
{

}