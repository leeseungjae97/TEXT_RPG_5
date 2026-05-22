//MP_Potion.h
#pragma once

#include "Item.h"

struct MP_Potion : public Item
{
	MP_Potion(string name, ItemType type, int price, int effectAmount = 0) : Item(name, type, price, effectAmount) {}

	void Use(Player* player) override
	{
		Item::Use(player);

		cout << "MP + " << EffectAmount << "\n";

	}
};