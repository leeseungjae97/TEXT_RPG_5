// HP_Potion.cpp

#include "HP_Potion.h"
#include "Player.h"



HP_Potion::HP_Potion(string name, ItemType type, int price, int effectAmount)
    : UItem(name, type, price, effectAmount) {}


void HP_Potion::Use(Player* player)
{
    UItem::Use(player);
    player->SetHP(min(player->GetMax_HP(), (player->GetHP() + EffectAmount)));
}