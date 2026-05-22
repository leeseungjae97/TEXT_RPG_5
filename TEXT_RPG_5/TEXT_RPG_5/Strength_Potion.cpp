// Strength_Potion.cpp
#include "Strength_Potion.h"
#include "Player.h"



Strength_Potion::Strength_Potion(string name, ItemType type, int price, int effectAmount)
    : UItem(name, type, price, effectAmount) {}


void Strength_Potion::Use(Player* player)
{
    UItem::Use(player);
}