//MP_Potion.h
#pragma once

#include "Item.h"

struct MP_Potion : public Item
{
	MP_Potion()
	{
		Name = "MP포션";
		Type = ItemType::Usable;
		Price = 50;
		EffectAmount = 50;
	}

	void Use(Player* player) override
	{
		Item::Use(player);

		cout << "MP + " << EffectAmount << "\n";

	}
};