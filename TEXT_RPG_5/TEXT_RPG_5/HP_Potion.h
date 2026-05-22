//HP_Potion.h
#pragma once

#include "Item.h"

struct HP_Potion : public Item
{
	HP_Potion(string name, ItemType type, int price, int effectAmount = 0) : Item(name, type, price, effectAmount) {}


	void Use(Player* player) override
	{
		Item::Use(player);

		cout << "HP + " << EffectAmount << "\n";

	}
};