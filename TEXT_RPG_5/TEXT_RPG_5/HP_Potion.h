//HP_Potion.h
#pragma once

#include "Item.h"

struct HP_Potion : public Item
{
	HP_Potion()
	{
		Name = "HP포션";
		Type = ItemType::Usable;
		Price = 50;
		EffectAmount = 50;
	}

	void Use(Player* player) override
	{
		Item::Use(player);

		cout << "HP + " << EffectAmount << "\n";

	}
};