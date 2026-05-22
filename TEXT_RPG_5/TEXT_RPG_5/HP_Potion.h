//HP_Potion.h
#pragma once

#include "Item.h"

struct HP_Potion : public UItem
{
	HP_Potion(string name, ItemType type, int price, int effectAmount = 0) : UItem(name, type, price, effectAmount) {}
	~HP_Potion() {}

	void Use(Player* player) override
	{
		UItem::Use(player);
		player->SetHP(max(player->GetMax_HP(), (player->GetHP()+EffectAmount)));
	}
};